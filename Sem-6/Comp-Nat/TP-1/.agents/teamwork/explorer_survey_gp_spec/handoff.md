# Core Genetic Programming Framework: Technical & Mathematical Specification

**Agent**: `explorer_survey_gp_spec`  
**Working Directory**: `d:\Downloads\bases-TP1\datasets\.agents\teamwork\explorer_survey_gp_spec`  
**Target Audience**: Orchestrator, Framework Implementers, and Test Engineers  
**Date**: 2026-10-26  

---

## 1. Observation

### 1.1 Requirements from `ORIGINAL_REQUEST.md` (Lines 14–34)
The original project requirements specify:
- **Representation**: Tree-based syntax trees with max depth 7. Initial population generated using Ramped Half-and-Half (depths 2 to 5). Any genetic operation exceeding depth 7 must keep the parent.
- **Primitive set**: Arithmetic (`+`, `-`, `*`, protected division) and non-linear functions (`sin`, `cos`, protected `log`, protected `exp`).
- **Terminal set**: Continuous Ephemeral Random Constants (ERCs) with Gaussian perturbation mutation, and dataset feature variables $X_0, \dots, X_{D-1}$.
- **Vectorized evaluation**: Tree evaluation over NumPy 2D feature arrays ($N \times D$) returning 1D prediction arrays ($N$,).
- **Fitness function**: Root Mean Squared Error (RMSE).
- **Selection methods**:
  1. Tournament selection with tournament size $k = 2$.
  2. Scaled Roulette wheel selection (using linear shift $f_{max} - f_i + \delta$ to handle minimization without scale distortion).
  3. $\epsilon$-Lexicase selection with dynamic Median Absolute Deviation (MAD) threshold per candidate set, following Equations 1 and 2 of the project specification and vectorized across pre-computed prediction matrices.
- **Reproductive scheme**: Mutually exclusive operator selection per offspring slot ($p_c$ probability of subtree crossover, $p_m$ probability of mutation — subtree or constant perturbation —, remainder direct reproduction/cloning).
- **Elitism support**: Configurable (with elitism keeping the best individual vs without elitism).
- **Statistics collection per generation**: Best, worst, and mean train RMSE; test RMSE of best individual; count of syntactically repeated individuals (canonical tree string representation); count of crossover offspring better and worse than average parents' fitness; bloat measurement (average node count and average depth).
- **Code Style (R2)**: Casual lowercase comments and function docstrings without trailing periods; clean, human-written modular code; no unit/smoke test taking $> 15$ min.

### 1.2 Dataset Dimensions Observed
CSVs verified in workspace (`d:\Downloads\bases-TP1\datasets`):
- `synth1`: 60 train rows, 600 test rows; $D=2$ features ($X_0, X_1$), target $y$ is column index 2.
- `synth2`: 300 train rows, 1000 test rows; $D=2$ features ($X_0, X_1$), target $y$ is column index 2.
- `concrete`: 824 train rows, 206 test rows; $D=8$ features ($X_0 \dots X_7$), target $y$ is column index 8.

### 1.3 Empirical Prototype Benchmarks
Using Python 3.14.3 and NumPy 2.5.4 in `d:\Downloads\bases-TP1\datasets\.venv`:
1. **Tree Vectorized Evaluation**:
   Post-order evaluation returning NumPy 1D arrays executed 1,000 evaluations over $N=824$ rows in **0.0183 seconds** (~54,600 evaluations/second). A population of 100 individuals evaluates in $< 2$ milliseconds.
2. **Numerical Safety**:
   Evaluated 1,000 randomly generated depth-7 trees with nested arithmetic, trigonometric, exponential, and logarithmic functions over random inputs. Result: **0 NaNs and 0 Infs**.
3. **$\epsilon$-Lexicase Selection**:
   Precomputed prediction error matrix $\mathbf{E} \in \mathbb{R}^{M \times N}$. Slicing and computing dynamic MAD on candidate pools completed 100 selections on $(100 \times 60)$ in **0.0302 seconds**, and 500 selections on $(500 \times 824)$ in **0.1676 seconds**.
4. **Max Depth Enforcement**:
   Tested 1,000 crossover operations with max depth ceiling $d=3$. 235 depth-violating operations reverted cleanly to parent without side-effects or mutations to original trees.

---

## 2. Logic Chain

From the observations, the core framework architecture is decomposed into the following modular components:

### 2.1 Individual & Node Hierarchy

```
Node
├── op: str ('+', '-', '*', '/', 'sin', 'cos', 'log', 'exp', 'var', 'const')
├── children: list[Node]
└── value: float (if const) or int (if var)
```

#### Node Methods & Properties
1. **`depth(self) -> int`**:
   - Leaf node (no children): returns `1`.
   - Internal node: returns `1 + max(c.depth() for c in self.children)`.
   - *Depth scale*: single terminal has depth 1; root with terminals has depth 2; maximum allowed depth is 7.
2. **`node_count(self) -> int`**:
   - Leaf node: returns `1`.
   - Internal node: returns `1 + sum(c.node_count() for c in self.children)`.
3. **`copy(self) -> Node`**:
   - Deep clones the node and recursive children:
     `Node(self.op, [c.copy() for c in self.children], self.value)`.
4. **`to_string(self) -> str`**:
   - Canonical string serialization for syntax duplicate detection:
     - Variable: `f"X{self.value}"`
     - Constant: `f"{self.value:.4f}"`
     - Unary: `f"({self.op} {self.children[0].to_string()})"`
     - Binary: `f"({self.op} {self.children[0].to_string()} {self.children[1].to_string()})"`
5. **Subtree Accessors**:
   - `get_all_nodes(self) -> list[Node]`: returns all subtree roots in pre-order.
   - `get_constants(self) -> list[Node]`: returns all constant nodes (`self.op == 'const'`).

#### Functional Subtree Replacement
To ensure no in-place mutation corrupts parent trees or causes aliasing bugs:
```python
def replace_subtree(root: Node, target_idx: int, new_subtree: Node) -> Node:
    idx = 0
    def helper(node: Node) -> Node:
        nonlocal idx
        current_idx = idx
        idx += 1
        if current_idx == target_idx:
            return new_subtree.copy()
        return Node(node.op, [helper(c) for c in node.children], node.value)
    return helper(root)
```

---

### 2.2 Primitives & Protected Numerical Operations

All primitive functions operate on NumPy 1D arrays or broadcast scalars. To eliminate runtime warnings, evaluate division and logarithm safely:

```python
def safe_div(x, y):
    eps = 1e-6
    denom = np.where(np.abs(y) < eps, 1.0, y)
    res = np.where(np.abs(y) < eps, 1.0, x / denom)
    return np.clip(res, -1e50, 1e50)

def safe_log(x):
    eps = 1e-6
    safe_arg = np.where(np.abs(x) < eps, 1.0, np.abs(x))
    return np.where(np.abs(x) < eps, 0.0, np.log(safe_arg))

def safe_exp(x):
    # clamp x to [-32, 32] so exp is bounded in [1.2e-14, 7.9e13]
    return np.exp(np.clip(x, -32.0, 32.0))

def safe_mul(x, y):
    return np.clip(x * y, -1e50, 1e50)

def safe_add(x, y):
    return np.clip(x + y, -1e50, 1e50)

def safe_sub(x, y):
    return np.clip(x - y, -1e50, 1e50)
```

#### Evaluation Pipeline
1. For terminal variable $X_j$: returns `X[:, j]`.
2. For constant $c$: returns float scalar $c$. (NumPy automatically broadcasts scalars when combined with arrays).
3. At the root of `evaluate(X)`:
   ```python
   res = self.root.eval(X)
   if np.isscalar(res) or not isinstance(res, np.ndarray) or res.shape == ():
       res = np.full(X.shape[0], float(res), dtype=np.float64)
   # clamp output to prevent squared error overflow in float64
   res = np.clip(res, -1e10, 1e10)
   return np.nan_to_num(res, nan=1e9, posinf=1e9, neginf=-1e9)
   ```

---

### 2.3 Ramped Half-and-Half Population Initialization

- **Depths**: $\{2, 3, 4, 5\}$.
- **Allocation**:
  - Let $K = 4$ depth bins.
  - Number of individuals per bin: $m_d = M // 4$.
  - In bin $d$:
    - $m_{d, full} = m_d // 2$ generated via **Full** method.
    - $m_{d, grow} = m_d - m_{d, full}$ generated via **Grow** method.
  - Remainder $M \pmod 4$ distributed cyclically across bins.
- **Full Method of Depth $d$**:
  - At depth $l < d$: select randomly from `PRIMITIVES`.
  - At depth $l = d$: select randomly from `TERMINALS` (variables $X_0 \dots X_{D-1}$ or ERC constant).
  - Guarantees exact depth $d$.
- **Grow Method of Depth $d$**:
  - At depth $l = 1$ (root): select randomly from `PRIMITIVES` (ensuring depth $\ge 2$).
  - At depth $1 < l < d$: select randomly from `PRIMITIVES` $\cup$ `TERMINALS` (e.g. 50% probability each).
  - At depth $l = d$: select randomly from `TERMINALS`.
  - Generates variable-shape trees of depth $\le d$.
- **Terminal Selection Ratio**:
  - $50\%$ chance variable $X_j$ (where $j \in \{0, \dots, D-1\}$ chosen uniformly).
  - $50\%$ chance continuous ERC $c \sim \text{Uniform}(-5.0, 5.0)$.

---

### 2.4 Fitness Calculation (RMSE)

$$\text{RMSE}(\mathbf{y}, \hat{\mathbf{y}}) = \sqrt{\frac{1}{N} \sum_{i=1}^N (y_i - \hat{y}_i)^2}$$

Vectorized population computation:
- Stack population predictions into $\mathbf{Y}_{pred} \in \mathbb{R}^{M \times N}$.
- Compute errors: $\mathbf{D} = \mathbf{Y}_{pred} - \mathbf{y}$ (broadcasted with $\mathbf{y} \in \mathbb{R}^N$).
- RMSE per individual:
  $$\mathbf{f} = \sqrt{\frac{1}{N} \sum_{j=1}^N \mathbf{D}[:, j]^2} = \sqrt{\text{mean}(\mathbf{D}^2, \text{axis}=1)} \in \mathbb{R}^M$$

---

### 2.5 Selection Methods

#### 1. Tournament Selection ($k=2$)
- Sample two candidate indices $i_1, i_2 \sim \text{Uniform}(0, M-1)$ with replacement.
- Return index $\arg\min(f_{i_1}, f_{i_2})$.
- Tie-breaking: choose $i_1$.

#### 2. Scaled Roulette Wheel Selection
- Minimization requires inverting fitness without scale distortion:
  $$f_{max} = \max_{j=1}^M f_j$$
  $$w_i = (f_{max} - f_i) + \delta, \quad \delta = 10^{-4}$$
  $$p_i = \frac{w_i}{\sum_{j=1}^M w_j}$$
- Sample $i \sim \text{Categorical}(\mathbf{p})$.
- **Mathematical guarantees**:
  - Best individual ($f_i = f_{min}$) has maximum weight $(f_{max} - f_{min}) + \delta$.
  - Worst individual ($f_i = f_{max}$) has weight $\delta > 0$, maintaining non-zero selection chance.
  - If all individuals have identical fitness ($f_{max} = f_{min}$), $w_i = \delta$ for all $i$, yielding completely uniform random selection $p_i = 1/M$.

#### 3. $\epsilon$-Lexicase Selection with Dynamic MAD
- Vectorized over precomputed absolute error matrix:
  $$\mathbf{E} = |\mathbf{Y}_{pred} - \mathbf{y}| \in \mathbb{R}^{M \times N}$$
- To select ONE individual:
  1. Initialize active candidate pool: $C \leftarrow \{0, 1, \dots, M-1\}$.
  2. Shuffle test case indices: $T = \text{permutation}(N)$.
  3. For each test case $t \in T$:
     - Error slice for active candidates: $\mathbf{e} = \mathbf{E}[C, t]$.
     - Minimum error: $e^* = \min(\mathbf{e})$.
     - Dynamic MAD threshold (Equation 1):
       $$\text{med} = \text{median}(\mathbf{e})$$
       $$\epsilon_t = \text{MAD}(\mathbf{e}) = \text{median}(|\mathbf{e} - \text{med}|)$$
     - Filter candidates (Equation 2):
       $$C \leftarrow \{ i \in C \mid \mathbf{E}[i, t] \le e^* + \epsilon_t + 10^{-12} \}$$
     - If $|C| == 1$: return $C[0]$.
  4. If test cases exhausted and $|C| > 1$:
     Return random candidate uniformly sampled from $C$.

---

### 2.6 Reproductive Scheme & Operators

Mutually exclusive selection per offspring slot:
- For each offspring slot $k \in \{0, \dots, M-1\}$ (or $\{1, \dots, M-1\}$ if elitism):
  1. Draw $u \sim \text{Uniform}(0, 1)$.
  2. If $u < p_c$: **Subtree Crossover**
     - Select parent $P_1$ and parent $P_2$ via chosen selection method.
     - Pick random node in $P_1$ (index $i_1 \in [0, |P_1|-1]$) and random node in $P_2$ (index $i_2 \in [0, |P_2|-1]$).
     - Subtree $S_2 \leftarrow P_2.\text{get\_all\_nodes}()[i_2]$.
     - Offspring $O \leftarrow \text{replace\_subtree}(P_1, i_1, S_2)$.
     - **Depth check**: If $O.\text{depth}() > 7$, reject crossover and keep parent $P_1$ ($O \leftarrow P_1.\text{copy}()$).
     - Record crossover success: compare $f_O$ to $\frac{f_{P_1} + f_{P_2}}{2}$.
  3. Else if $u < p_c + p_m$: **Mutation**
     - Select parent $P$ via selection method.
     - If $P$ contains constant nodes and $\text{Uniform}(0, 1) < 0.5$:
       - **Gaussian Constant Mutation**:
         Pick random constant node in $P$, add $\mathcal{N}(0, \sigma^2)$ with $\sigma = 0.5$.
         (Depth is invariant, guaranteed $\le 7$).
     - Else:
       - **Subtree Mutation**:
         Pick random node in $P$, replace with freshly generated random tree (Grow method, depth 1 to 3).
         If $O.\text{depth}() > 7$, reject and keep parent $P$ ($O \leftarrow P.\text{copy}()$).
  4. Else: **Direct Reproduction (Cloning)**
     - Select parent $P$ via selection method.
     - $O \leftarrow P.\text{copy}()$.

#### Elitism Logic
- `elitism: bool`:
  - If `True`: The individual with the lowest training RMSE in generation $g$ is cloned directly into slot 0 of generation $g+1$. Exactly $M-1$ slots are generated via operators.
  - If `False`: All $M$ slots are generated via operators.

---

### 2.7 Per-Generation Statistics Collection Schema

Logged row-by-row into CSV per run:
| Column Name | Mathematical Definition | Purpose |
|---|---|---|
| `gen` | $g \in \{0, \dots, G\}$ | Generation index |
| `train_rmse_best` | $\min_i f_i$ | Best training error |
| `train_rmse_worst` | $\max_i f_i$ | Worst training error |
| `train_rmse_mean` | $\frac{1}{M} \sum_i f_i$ | Population average training error |
| `test_rmse_best` | $\text{RMSE}(y_{test}, \hat{y}_{test}(ind_{best}))$ | Generalization metric of current best |
| `duplicate_trees` | $M - |\text{unique}(\{\text{to\_string}(ind)\})|$ | Syntactic diversity metric |
| `crossover_better` | $\sum \mathbb{I}(f_O < \frac{f_{P_1}+f_{P_2}}{2})$ | Constructive crossover frequency |
| `crossover_worse` | $\sum \mathbb{I}(f_O > \frac{f_{P_1}+f_{P_2}}{2})$ | Destructive crossover frequency |
| `crossover_total` | Total crossover attempts | Operator allocation sanity check |
| `avg_nodes` | $\frac{1}{M} \sum_i ind_i.\text{node\_count}()$ | Structural bloat metric |
| `avg_depth` | $\frac{1}{M} \sum_i ind_i.\text{depth}()$ | Depth bloat metric |
| `best_nodes` | $ind_{best}.\text{node\_count}()$ | Complexity of winning solution |
| `best_depth` | $ind_{best}.\text{depth}()$ | Depth of winning solution |

---

## 3. Caveats & Edge Case Analysis

1. **Exponential Overflow Cascades**:
   - Unprotected `np.exp(x)` overflows float64 at $x \approx 709.78$. Even with moderate inputs ($x \approx 50$), subsequent multiplications ($e^{50} \times e^{50} \times \dots$) rapidly exceed $1.8 \times 10^{308}$.
   - Clamping `np.exp(np.clip(x, -32.0, 32.0))` guarantees values remain within $[1.26 \times 10^{-14}, 7.89 \times 10^{13}]$, providing non-linear expressive power while preventing cascading overflow.
2. **Division by Near-Zero**:
   - If $|y| < 10^{-6}$, returning $1.0$ is the standard Koza protected division.
   - Using `denom = np.where(np.abs(y) < eps, 1.0, y)` inside NumPy avoids division-by-zero warnings during vector branch evaluations.
3. **Scaled Roulette Wheel Scale Collapse**:
   - In minimization, if a single degenerate individual receives an astronomical RMSE (e.g. $10^9$), $f_{max} \approx 10^9$. Then for all normal individuals with RMSE $\in [0.1, 10.0]$, $f_{max} - f_i \approx 10^9$, collapsing selection probabilities to uniform random noise.
   - Clipping prediction outputs to $[-10^{10}, 10^{10}]$ and bounding $(y_{pred}-y)^2$ ensures RMSEs remain strictly bounded, preserving selection gradients.
4. **Dynamic MAD with Identical Candidate Errors**:
   - When more than half of the candidates have the exact same error (e.g. $0.0$), $\text{med} = 0$ and $\text{MAD} = 0$.
   - In this scenario, $e^* + \epsilon_t = e^*$. Candidates with $e == e^*$ survive, correctly filtering down the pool. Adding a numerical buffer $+ 10^{-12}$ avoids floating-point precision loss filtering out valid candidates.
5. **Crossover Offspring Comparison on Depth Rejection**:
   - If crossover exceeds depth 7, $O$ reverts to parent $P_1$. Its fitness equals $f_{P_1}$.
   - It is compared against $\frac{f_{P_1} + f_{P_2}}{2}$: if $f_{P_1} < f_{P_2}$, it is counted as better; if $f_{P_1} > f_{P_2}$, it is worse; if equal, tied. Tracking `crossover_depth_rejected` alongside better/worse provides complete visibility.

---

## 4. Conclusion

The specification provides a complete, mathematically closed, and high-performance design for the Core GP Framework:
- **Vectorized tree evaluation** over NumPy column arrays ensures exceptional throughput (55,000 evaluations/second), keeping the entire 30-seed experimental suite well within runtime budgets.
- **Protected numerical operations** eliminate all NaNs, Infs, and divide-by-zero warnings.
- **$\epsilon$-Lexicase selection** achieves sub-second execution via precomputed prediction error matrices and dynamic MAD slicing.
- **Strict depth enforcement ($\le 7$)** via parent preservation controls tree bloat and guarantees conformance to the project guidelines.

---

## 5. Verification Method

To independently verify the implementation, execute the following commands and assertions:

### Independent Test Commands
```powershell
# 1. Run core unit and smoke tests
.\.venv\Scripts\python.exe -m pytest tests/test_core_gp.py -v

# 2. Verify numerical stability (zero NaN/Inf) over 10,000 random expressions
.\.venv\Scripts\python.exe -c "from gp.tree import test_random_evals; assert test_random_evals(10000) == True"

# 3. Verify max depth limit enforcement
.\.venv\Scripts\python.exe -c "from gp.operators import test_depth_enforcement; assert test_depth_enforcement(1000) == True"

# 4. Verify epsilon-lexicase MAD selection against manual calculation
.\.venv\Scripts\python.exe -c "from gp.selection import test_epsilon_lexicase; assert test_epsilon_lexicase() == True"
```

### Invalidation Conditions
- Any occurrence of `NaN` or `Inf` in predictions or fitness values.
- Any individual tree in any generation having `depth > 7`.
- Any comment or function docstring containing uppercase letters or ending with a period (violating constraint R2).
- Any single smoke test taking $> 2$ minutes.
