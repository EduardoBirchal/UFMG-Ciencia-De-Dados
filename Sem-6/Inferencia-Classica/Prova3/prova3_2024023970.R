set.seed(2024023970)

# inicializando dados
alpha0 <- runif(1, 2.5, 6)
beta0 <- runif(1, 0.5, 3)
x <- rgamma(50, shape = alpha0, rate = beta0)


# parâmetros e estatísticas
media <- mean(x)
var <- var(x)

cat("PARÂMETROS E ESTATÍSTICAS:\n")
cat(sprintf("alpha0: %.4f\n", alpha0))
cat(sprintf("beta0: %.4f\n", beta0))
cat(sprintf("Média amostral: %.4f\n", media))
cat(sprintf("Variância amostral: %.4f\n", var))


# histograma da amostra

hist_info <- hist(x, plot = FALSE)
y_max <- max(hist_info$density, dgamma(seq(0, max(x) * 1.2, length.out = 200), 
                                       shape = alpha0, rate = beta0))

hist(x,
     freq   = FALSE, 
     ylim   = c(0, y_max * 1.1),
     xlim   = c(0, max(x) * 1.15),
     col    = "lightblue",
     border = "white",
     main   = "Amostra vs Gama Teórica",
     xlab   = "x",
     ylab   = "Densidade")

curve(dgamma(x, shape = alpha0, rate = beta0),
      from = 0,
      to   = max(x) * 1.15,
      col  = "red",
      lwd  = 2,
      add  = TRUE)

legend("topright",
       legend = c("Amostra", sprintf("Gama(%.4f, %.4f)", alpha0, beta0)),
       fill   = c("lightblue", NA),
       border = c("white", NA),
       lty    = c(NA, 1),
       lwd    = c(NA, 2),
       col    = c(NA, "red"),
       bty    = "n")



# estimador de momentos

estimadores_momentos <- function(x) {
  n <- length(x)
  media_amostral <- mean(x)
  
  var_amostral_n <- sum((x - media_amostral)^2) / n
  
  # isolando alpha e beta a partir dos dois primeiros momentos
  alpha_estimado <- (media_amostral^2) / var_amostral_n
  beta_estimado  <- media_amostral / var_amostral_n
  
  return(list(
    alpha_estimado = alpha_estimado,
    beta_estimado  = beta_estimado
  ))
}

est_mom <- estimadores_momentos(x)

alpha_estimado <- est_mom$alpha_estimado
beta_estimado  <- est_mom$beta_estimado

cat("\nRESULTADOS DO MÉTODO DOS MOMENTOS\n")
cat(sprintf("alpha estimado: %.4f\n", alpha_estimado))
cat(sprintf("beta estimado : %.4f\n", beta_estimado))


# algoritmo de newton-raphson

nr_gama <- function(x, alpha_init, tol = 1e-8, max_iter = 100) {
  media_amostral <- mean(x)
  s <- log(media_amostral) - mean(log(x))
  
  alpha_atual <- alpha_init
  convergiu <- FALSE
  saiu_espaco <- FALSE
  
  # histórico das iterações 
  historico <- data.frame(
    k = integer(),
    alpha_k = numeric(),
    g_alpha_k = numeric(),
    diff_alpha = numeric()
  )
  
  for (k in 1:max_iter) {
    # g(alpha) e g'(alpha)
    g_val  <- log(alpha_atual) - digamma(alpha_atual) - s
    dg_val <- (1 / alpha_atual) - trigamma(alpha_atual)
    
    # passo de atualização
    alpha_prox <- alpha_atual - (g_val / dg_val)
    

    if (is.nan(alpha_prox) || is.infinite(alpha_prox) || alpha_prox <= 0) {
      saiu_espaco <- TRUE
      historico <- rbind(historico, data.frame(
        k = k,
        alpha_k = alpha_prox,
        g_alpha_k = NA,
        diff_alpha = abs(alpha_prox - alpha_atual)
      ))
      break
    }
    
    diff_atual <- abs(alpha_prox - alpha_atual)
    g_prox <- log(alpha_prox) - digamma(alpha_prox) - s
    
    # registra a iteração
    historico <- rbind(historico, data.frame(
      k = k,
      alpha_k = alpha_prox,
      g_alpha_k = g_prox,
      diff_alpha = diff_atual
    ))
    

    if (diff_atual < tol) {
      alpha_atual <- alpha_prox
      convergiu <- TRUE
      break
    }
    
    alpha_atual <- alpha_prox
  }
  
  # beta_hat = alpha_hat / media
  if (convergiu) {
    alpha_hat <- alpha_atual
    beta_hat  <- alpha_hat / media_amostral
  } else {
    alpha_hat <- NA
    beta_hat  <- NA
  }
  
  return(list(
    alpha_hat   = alpha_hat,
    beta_hat    = beta_hat,
    iteracoes   = nrow(historico),
    convergiu   = convergiu,
    saiu_espaco = saiu_espaco,
    historico   = historico
  ))
}


# newton raphson partindo do estimador de momentos

res_mom <- nr_gama(x, alpha_init = alpha_estimado, tol = 1e-8)

cat("\nRESULTADOS DO NEWTON-RAPHSON:\n")
cat(sprintf("Convergiu? %s | Iterações: %d\n", res_mom$convergiu, res_mom$iteracoes))
cat(sprintf("EMV alpha_hat: %.6f\n", res_mom$alpha_hat))
cat(sprintf("EMV beta_hat : %.6f\n\n", res_mom$beta_hat))

cat("Tabela de iterações (k, alpha_k, g(alpha_k), |alpha_k - alpha_{k-1}|):\n")
print(res_mom$historico, row.names = FALSE, digits = 8)


# testando sensibilidade ao ponto inicial

res_001 <- nr_gama(x, alpha_init = 0.01, tol = 1e-8)
res_50  <- nr_gama(x, alpha_init = 50,   tol = 1e-8)

cat("\nTESTE DE SENSIBILIDADE AO PONTO INICIAL:\n")
cat(sprintf("Início = 0.01 -> Convergiu: %s | Iterações: %d | Saiu do espaço (alpha <= 0): %s | alpha_hat: %.6f\n",
            res_001$convergiu, res_001$iteracoes, res_001$saiu_espaco, res_001$alpha_hat))
cat(sprintf("Início = 50   -> Convergiu: %s | Iterações: %d | Saiu do espaço (alpha <= 0): %s | alpha_hat: %.6f\n",
            res_50$convergiu, res_50$iteracoes, res_50$saiu_espaco, res_50$alpha_hat))



# comparação entre momentos, EMV e parâmetros verdadeiros

tabela_comp <- data.frame(
  Parametro  = c("alpha (forma)", "beta (taxa)"),
  Verdadeiro = c(alpha0, beta0),
  Momentos   = c(alpha_estimado, beta_estimado),
  EMV        = c(res_mom$alpha_hat, res_mom$beta_hat)
)

cat("\nCOMPARAÇÃO DOS ESTIMADORES:\n")
print(tabela_comp, row.names = FALSE, digits = 4)



# intervalos de confiança assintóticos

# calcula erro padrao estimado e intervalo de confiança de wald 
ic_wald_gama <- function(alpha_hat, beta_hat, n, nivel_sig = 0.05) {
  z <- qnorm(1 - nivel_sig / 2)
  denom <- n * (alpha_hat * trigamma(alpha_hat) - 1)
  
  ep_alpha <- sqrt(alpha_hat / denom)
  ep_beta  <- beta_hat * sqrt(trigamma(alpha_hat) / denom)
  
  ic_alpha <- c(alpha_hat - z * ep_alpha, alpha_hat + z * ep_alpha)
  ic_beta  <- c(beta_hat  - z * ep_beta,  beta_hat  + z * ep_beta)
  
  return(list(
    ep_alpha = ep_alpha,
    ep_beta  = ep_beta,
    ic_alpha = ic_alpha,
    ic_beta  = ic_beta
  ))
}

# aplicando à amostra 
res_ic <- ic_wald_gama(res_mom$alpha_hat, res_mom$beta_hat, n = length(x), nivel_sig = 0.05)

contem_alpha <- (alpha0 >= res_ic$ic_alpha[1]) && (alpha0 <= res_ic$ic_alpha[2])
contem_beta  <- (beta0  >= res_ic$ic_beta[1])  && (beta0  <= res_ic$ic_beta[2])

cat("\nINTERVALOS DE CONFIANÇA:\n")
cat(sprintf("alpha0 = %.4f | EP(alpha_hat) = %.4f | IC 95%%: [%.4f, %.4f] | Contém alpha0? %s\n",
            alpha0, res_ic$ep_alpha, res_ic$ic_alpha[1], res_ic$ic_alpha[2], contem_alpha))
cat(sprintf("beta0  = %.4f | EP(beta_hat)  = %.4f | IC 95%%: [%.4f, %.4f] | Contém beta0?  %s\n",
            beta0, res_ic$ep_beta, res_ic$ic_beta[1], res_ic$ic_beta[2], contem_beta))



# simulação de monte carlo

B <- 5000
tamanhos_n <- c(20, 50, 200)

tabela_cobertura <- data.frame()
tabela_estimativas <- data.frame()

for (n_atual in tamanhos_n) {
  cat(sprintf("\nIniciando simulação de monte carlo com n = %i... ", n_atual))
  
  # vetores para armazenar os resultados das B réplicas
  alpha_hats <- numeric(B)
  beta_hats  <- numeric(B)
  cob_alpha  <- logical(B)
  cob_beta   <- logical(B)
  comp_alpha <- numeric(B)
  comp_beta  <- numeric(B)
  nao_conv   <- 0
  
  for (b in 1:B) {
    # gerar amostra da gama
    amostra_b <- rgamma(n_atual, shape = alpha0, rate = beta0)
    
    # newton-raphson
    mom_b <- estimadores_momentos(amostra_b)
    nr_b  <- nr_gama(amostra_b, alpha_init = mom_b$alpha_estimado, tol = 1e-8)
    
    # se não convergiu, registra falha e atribui NA
    if (!nr_b$convergiu) {
      nao_conv      <- nao_conv + 1
      alpha_hats[b] <- NA
      beta_hats[b]  <- NA
      cob_alpha[b]  <- NA
      cob_beta[b]   <- NA
      comp_alpha[b] <- NA
      comp_beta[b]  <- NA
    } else {
      alpha_hats[b] <- nr_b$alpha_hat
      beta_hats[b]  <- nr_b$beta_hat
      
      # intervalos de confiança
      ic_b <- ic_wald_gama(nr_b$alpha_hat, nr_b$beta_hat, n = n_atual, nivel_sig = 0.05)
      
      
      cob_alpha[b]  <- (alpha0 >= ic_b$ic_alpha[1]) && (alpha0 <= ic_b$ic_alpha[2])
      cob_beta[b]   <- (beta0  >= ic_b$ic_beta[1])  && (beta0  <= ic_b$ic_beta[2])
      comp_alpha[b] <- ic_b$ic_alpha[2] - ic_b$ic_alpha[1]
      comp_beta[b]  <- ic_b$ic_beta[2]  - ic_b$ic_beta[1]
    }
  }
  
  cat("\tfinalizado.")
  
  B_validos <- B - nao_conv
  
  # metricas para alpha
  p_hat_alpha  <- mean(cob_alpha, na.rm = TRUE)
  ep_mc_alpha  <- sqrt(p_hat_alpha * (1 - p_hat_alpha) / B_validos)
  comp_m_alpha <- mean(comp_alpha, na.rm = TRUE)
  
  # métricas para beta
  p_hat_beta   <- mean(cob_beta, na.rm = TRUE)
  ep_mc_beta   <- sqrt(p_hat_beta * (1 - p_hat_beta) / B_validos)
  comp_m_beta  <- mean(comp_beta, na.rm = TRUE)
  
  # adiciona linha na tabela de cobertura e intervalos
  tabela_cobertura <- rbind(tabela_cobertura, data.frame(
    n                 = n_atual,
    Parametro         = c("alpha", "beta"),
    Cobertura_Est     = c(p_hat_alpha, p_hat_beta),
    EP_Monte_Carlo    = c(ep_mc_alpha, ep_mc_beta),
    Comprimento_Medio = c(comp_m_alpha, comp_m_beta),
    Nao_Convergiu     = c(nao_conv, nao_conv)
  ))
  
  # adiciona linha na tabela de média e desvio padrão dos EMVs
  tabela_estimativas <- rbind(tabela_estimativas, data.frame(
    n               = n_atual,
    Media_alpha_hat = mean(alpha_hats, na.rm = TRUE),
    DP_alpha_hat    = sd(alpha_hats, na.rm = TRUE),
    Media_beta_hat  = mean(beta_hats, na.rm = TRUE),
    DP_beta_hat     = sd(beta_hats, na.rm = TRUE)
  ))
}

cat("\n\nRESULTADOS DA SIMULAÇÃO DE MONTE CARLO:\n")
print(tabela_cobertura, row.names = FALSE, digits = 4)

cat("\nMÉDIA E DESVIO PADRÃO DAS RÉPLICAS:\n")
print(tabela_estimativas, row.names = FALSE, digits = 4)