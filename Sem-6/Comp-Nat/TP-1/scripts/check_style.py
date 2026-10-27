"""verificador de estilo checando comentários e docstrings casuais em minúsculas sem ponto final"""

import ast
import os
import sys
import tokenize


def check_file(file_path: str) -> list[str]:
    """verifica se um único arquivo python possui comentários em maiúsculas ou pontos finais"""
    errors = []

    # verifica comentários usando tokenize
    try:
        with open(file_path, "rb") as f:
            tokens = tokenize.tokenize(f.readline)
            for tok in tokens:
                if tok.type == tokenize.COMMENT:
                    raw = tok.string
                    # remove cerquilhas iniciais e espaços em branco
                    content = raw.lstrip("#").strip()
                    if not content:
                        continue
                    if content != content.lower():
                        errors.append(f"{file_path}:{tok.start[0]} comment has uppercase: '{raw}'")
                    if content.endswith("."):
                        errors.append(f"{file_path}:{tok.start[0]} comment ends with period: '{raw}'")
    except Exception as exc:
        errors.append(f"{file_path}:0 tokenizer error: {exc}")

    # verifica docstrings usando ast
    try:
        with open(file_path, "r", encoding="utf-8") as f:
            tree = ast.parse(f.read(), filename=file_path)

        for node in ast.walk(tree):
            if isinstance(node, (ast.Module, ast.FunctionDef, ast.AsyncFunctionDef, ast.ClassDef)):
                if node.body and isinstance(node.body[0], ast.Expr):
                    val = node.body[0].value
                    if isinstance(val, ast.Constant) and isinstance(val.value, str):
                        doc = val.value.strip()
                        lineno = node.body[0].lineno
                        if doc:
                            if doc != doc.lower():
                                errors.append(f"{file_path}:{lineno} docstring has uppercase: '{doc[:30]}...'")
                            # verifica se a linha da docstring termina com ponto
                            lines = [ln.strip() for ln in doc.splitlines() if ln.strip()]
                            for line in lines:
                                if line.endswith("."):
                                    errors.append(f"{file_path}:{lineno} docstring line ends with period: '{line}'")
                                    break
    except Exception as exc:
        errors.append(f"{file_path}:0 ast parser error: {exc}")

    return errors


def find_python_files(targets: list[str]) -> list[str]:
    """coleta todos os arquivos python nos alvos fornecidos ignorando diretórios desconsiderados"""
    ignored_dirs = {".venv", "venv", "env", ".git", ".agents", "__pycache__", "build", "dist", ".pytest_cache"}
    files = []

    for target in targets:
        if os.path.isfile(target):
            if target.endswith(".py"):
                files.append(os.path.abspath(target))
        elif os.path.isdir(target):
            for root, dirs, filenames in os.walk(target):
                dirs[:] = [d for d in dirs if d not in ignored_dirs and not d.startswith(".")]
                for fname in filenames:
                    if fname.endswith(".py"):
                        files.append(os.path.abspath(os.path.join(root, fname)))

    return sorted(set(files))


def main() -> int:
    """executa o verificador de estilo sobre alvos especificados ou diretórios padrão do projeto"""
    targets = sys.argv[1:]
    if not targets:
        # padrão para escanear pastas de código-fonte, testes e scripts do projeto
        project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
        targets = [
            os.path.join(project_root, "src"),
            os.path.join(project_root, "scripts"),
            os.path.join(project_root, "tests"),
        ]
        # mantém apenas caminhos existentes
        targets = [t for t in targets if os.path.exists(t)]

    py_files = find_python_files(targets)
    all_errors = []

    for fpath in py_files:
        errs = check_file(fpath)
        if errs:
            all_errors.extend(errs)

    if all_errors:
        print(f"style check failed with {len(all_errors)} violation(s):", file=sys.stderr)
        for err in all_errors:
            print(f"  {err}", file=sys.stderr)
        return 1

    print(f"all {len(py_files)} files passed style checks")
    return 0


if __name__ == "__main__":
    sys.exit(main())
