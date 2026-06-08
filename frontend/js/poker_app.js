let socket = null;

let cartasSelecionadas = [];
let faseAtual = "MENU";

let modoSelecionado = "computador";
let quantidadeJogadoresSelecionada = 1;

let ultimaMaoJogador = [];

// ===============================
// CONTROLE DO MENU INICIAL
// ===============================

function configurarMenuInicial() {
    const botoesModo = document.querySelectorAll(".botao-modo");
    const avisoModo = document.getElementById("aviso-modo");
    const btnIniciar = document.getElementById("btn-iniciar-poker");
    const btnVoltarMenuPoker = document.getElementById("btn-voltar-menu-poker");

    botoesModo.forEach((botao) => {
        botao.addEventListener("click", () => {
            botoesModo.forEach((b) => b.classList.remove("ativo"));
            botao.classList.add("ativo");

            modoSelecionado = botao.dataset.modo;
            quantidadeJogadoresSelecionada = Number(botao.dataset.jogadores);

            atualizarAvisoModo(avisoModo);
        });
    });

    btnIniciar.addEventListener("click", () => {
        iniciarModoSelecionado();
    });

    btnVoltarMenuPoker.addEventListener("click", () => {
        voltarParaMenuPoker();
    });
}

function atualizarAvisoModo(avisoModo) {
    if (modoSelecionado === "computador") {
        avisoModo.textContent = "Modo selecionado: Contra computador.";
        return;
    }

    if (modoSelecionado === "solo") {
        avisoModo.textContent =
            "Modo selecionado: Treino individual. Será integrado na próxima etapa.";
        return;
    }

    avisoModo.textContent =
        `Modo selecionado: ${quantidadeJogadoresSelecionada} jogadores. ` +
        "Multiplayer será integrado na próxima etapa.";
}

function iniciarModoSelecionado() {
    if (modoSelecionado !== "computador") {
        const avisoModo = document.getElementById("aviso-modo");

        avisoModo.textContent =
            "Este modo já está no menu, mas a lógica multiplayer ainda será integrada ao servidor.";

        avisoModo.classList.add("aviso-destaque");

        setTimeout(() => {
            avisoModo.classList.remove("aviso-destaque");
        }, 1200);

        return;
    }

    mostrarTelaJogo();

    if (!socket || socket.readyState === WebSocket.CLOSED) {
        conectar();
    } else if (socket.readyState === WebSocket.OPEN) {
        enviarAcao("NOVA_RODADA");
    }
}

function mostrarTelaJogo() {
    document.getElementById("tela-menu").classList.add("escondida");
    document.getElementById("tela-jogo").classList.remove("escondida");

    document.getElementById("descricao-partida").textContent =
        "Mesa contra computador.";

    document.getElementById("titulo-adversario").textContent = "Computador";
    document.getElementById("descricao-adversario").textContent =
        "Cartas ocultas até o resultado.";

    faseAtual = "CARREGANDO";
    limparAreaTroca();
}

function voltarParaMenuPoker() {
    document.getElementById("tela-jogo").classList.add("escondida");
    document.getElementById("tela-menu").classList.remove("escondida");

    faseAtual = "MENU";
    cartasSelecionadas = [];
    ultimaMaoJogador = [];

    limparAreaTroca();

    if (socket && socket.readyState === WebSocket.OPEN) {
        socket.close();
    }
}

// ===============================
// CONEXÃO COM O SERVIDOR
// ===============================

function conectar() {
    const protocolo = window.location.protocol === "https:" ? "wss" : "ws";

    socket = new WebSocket(`${protocolo}://${window.location.host}/ws/poker`);

    socket.onopen = () => {
        document.getElementById("status").textContent =
            "Conectado ao servidor do Poker.";

        enviarAcao("OBTER_ESTADO_ATUAL");
    };

    socket.onmessage = (event) => {
        const estado = JSON.parse(event.data);
        atualizarTela(estado);
    };

    socket.onclose = () => {
        document.getElementById("status").textContent =
            "Conexão encerrada com o servidor.";
    };

    socket.onerror = () => {
        document.getElementById("status").textContent =
            "Erro na conexão com o servidor.";
    };
}

function enviarAcao(acao, dados = {}) {
    if (socket && socket.readyState === WebSocket.OPEN) {
        socket.send(JSON.stringify({
            acao: acao,
            ...dados
        }));
    }
}

// ===============================
// ATUALIZAÇÃO DA TELA DO JOGO
// ===============================

function atualizarTela(estado) {
    faseAtual = estado.fase;
    cartasSelecionadas = [];

    ultimaMaoJogador = estado.jogador.mao || [];

    atualizarPlacar(estado);
    atualizarMensagens(estado);
    atualizarTrocas(estado);

    desenharMao("mao-computador", estado.computador.mao, false);
    desenharMao("mao-jogador", estado.jogador.mao, true);

    document.getElementById("jogada-jogador").textContent =
        estado.jogador.jogada;

    document.getElementById("jogada-computador").textContent =
        estado.computador.jogada;

    atualizarContadorSelecionadas();
    atualizarBotoes();
    atualizarClasseDaFase();

    if (faseAtual === "ESCOLHENDO_TROCAS") {
        limparAreaTroca();
    }

    if (faseAtual === "RESULTADO") {
        marcarShowdown();
    }
}

function atualizarPlacar(estado) {
    document.getElementById("rodada").textContent = estado.rodada;
    document.getElementById("placar-jogador").textContent = estado.placar.jogador;
    document.getElementById("placar-computador").textContent = estado.placar.computador;
    document.getElementById("placar-empates").textContent = estado.placar.empates;
}

function atualizarMensagens(estado) {
    document.getElementById("mensagem").textContent = estado.mensagem || "";

    if (estado.fase === "RESULTADO") {
        if (estado.vencedor === "Jogador") {
            document.getElementById("vencedor").textContent =
                "Você venceu esta rodada!";
        } else if (estado.vencedor === "Computador") {
            document.getElementById("vencedor").textContent =
                "O computador venceu esta rodada.";
        } else {
            document.getElementById("vencedor").textContent =
                "Empate na rodada.";
        }

        return;
    }

    document.getElementById("vencedor").textContent =
        "Escolha suas cartas e confirme a troca para revelar o resultado.";
}

function atualizarTrocas(estado) {
    const trocasJogador = estado.trocas ? estado.trocas.jogador : 0;
    const trocasComputador = estado.trocas ? estado.trocas.computador : 0;

    document.getElementById("tag-trocas-jogador").textContent =
        `${trocasJogador} troca(s)`;

    document.getElementById("tag-trocas-computador").textContent =
        `${trocasComputador} troca(s)`;
}

function atualizarClasseDaFase() {
    document.body.classList.remove("fase-escolha", "fase-resultado", "fase-menu");

    if (faseAtual === "MENU") {
        document.body.classList.add("fase-menu");
    }

    if (faseAtual === "ESCOLHENDO_TROCAS") {
        document.body.classList.add("fase-escolha");
    }

    if (faseAtual === "RESULTADO") {
        document.body.classList.add("fase-resultado");
    }
}

// ===============================
// DESENHO DAS CARTAS
// ===============================

function desenharMao(idElemento, mao, clicavel) {
    const container = document.getElementById(idElemento);

    container.innerHTML = "";

    mao.forEach((carta, indice) => {
        const elementoCarta = criarElementoCarta(carta, indice, clicavel);
        container.appendChild(elementoCarta);
    });
}

function criarElementoCarta(carta, indice, clicavel) {
    const div = document.createElement("div");

    div.classList.add("carta");
    div.dataset.indice = indice;

    if (carta.oculta) {
        div.classList.add("oculta");

        div.innerHTML = `
            <div class="verso-carta">
                <span>♠</span>
                <span>♦</span>
            </div>
        `;

        return div;
    }

    div.classList.add(carta.cor);

    div.innerHTML = `
        <div class="canto topo">
            <span>${carta.valor_texto}</span>
            <span>${carta.simbolo}</span>
        </div>

        <div class="centro">
            ${carta.simbolo}
        </div>

        <div class="canto base">
            <span>${carta.valor_texto}</span>
            <span>${carta.simbolo}</span>
        </div>
    `;

    if (clicavel && faseAtual === "ESCOLHENDO_TROCAS") {
        div.classList.add("clicavel");

        div.addEventListener("click", () => {
            alternarSelecao(indice, div);
        });
    }

    return div;
}

function alternarSelecao(indice, elemento) {
    const posicao = cartasSelecionadas.indexOf(indice);

    if (posicao >= 0) {
        cartasSelecionadas.splice(posicao, 1);
        elemento.classList.remove("selecionada");
    } else {
        if (cartasSelecionadas.length >= 3) {
            document.getElementById("status").textContent =
                "Você pode trocar no máximo 3 cartas.";
            return;
        }

        cartasSelecionadas.push(indice);
        elemento.classList.add("selecionada");
    }

    atualizarContadorSelecionadas();
    atualizarAreaTroca();
}

function atualizarContadorSelecionadas() {
    document.getElementById("selecionadas").textContent =
        `Cartas selecionadas: ${cartasSelecionadas.length}/3`;
}

// ===============================
// ÁREA CENTRAL DE TROCA
// ===============================

function limparAreaTroca() {
    const slots = document.querySelectorAll(".slot-troca");

    slots.forEach((slot) => {
        slot.innerHTML = "";
        slot.classList.remove("preenchido");
    });
}

function atualizarAreaTroca() {
    limparAreaTroca();

    const slots = document.querySelectorAll(".slot-troca");

    cartasSelecionadas.forEach((indice, posicao) => {
        if (!slots[posicao]) {
            return;
        }

        const carta = ultimaMaoJogador[indice];

        if (!carta || carta.oculta) {
            return;
        }

        slots[posicao].classList.add("preenchido");

        slots[posicao].innerHTML = `
            <div class="mini-carta-troca ${carta.cor}">
                <span>${carta.valor_texto}</span>
                <strong>${carta.simbolo}</strong>
            </div>
        `;
    });
}

function animarTrocaParaMesa() {
    const cartas = document.querySelectorAll("#mao-jogador .carta");

    cartasSelecionadas.forEach((indice) => {
        if (cartas[indice]) {
            cartas[indice].classList.add("trocando");
        }
    });

    document.querySelector(".area-troca").classList.add("area-troca-ativa");

    setTimeout(() => {
        document.querySelector(".area-troca").classList.remove("area-troca-ativa");
    }, 700);
}

function marcarShowdown() {
    const cartasComputador = document.querySelectorAll("#mao-computador .carta");

    cartasComputador.forEach((carta, indice) => {
        carta.style.animationDelay = `${indice * 0.08}s`;
        carta.classList.add("revelada");
    });
}

// ===============================
// BOTÕES E AÇÕES
// ===============================

function atualizarBotoes() {
    const podeTrocar = faseAtual === "ESCOLHENDO_TROCAS";
    const resultadoRevelado = faseAtual === "RESULTADO";

    const btnTrocar = document.getElementById("btn-trocar");
    const btnNovaRodada = document.getElementById("btn-nova-rodada");

    btnTrocar.disabled = !podeTrocar;

    if (podeTrocar) {
        btnTrocar.textContent = "Confirmar troca";
        btnNovaRodada.textContent = "Reiniciar rodada";
    }

    if (resultadoRevelado) {
        btnTrocar.textContent = "Troca finalizada";
        btnNovaRodada.textContent = "Próxima rodada";
    }
}

function configurarBotoesDoJogo() {
    document.getElementById("btn-trocar").addEventListener("click", () => {
        confirmarTrocaComAnimacao();
    });

    document.getElementById("btn-nova-rodada").addEventListener("click", () => {
        limparAreaTroca();
        enviarAcao("NOVA_RODADA");
    });
}

function confirmarTrocaComAnimacao() {
    if (faseAtual !== "ESCOLHENDO_TROCAS") {
        return;
    }

    animarTrocaParaMesa();

    document.getElementById("status").textContent =
        cartasSelecionadas.length === 0
            ? "Nenhuma carta trocada. Revelando resultado..."
            : "Cartas enviadas para a área de troca...";

    setTimeout(() => {
        enviarAcao("TROCAR_CARTAS", {
            indices: cartasSelecionadas
        });
    }, 520);
}

// ===============================
// INICIALIZAÇÃO
// ===============================

configurarMenuInicial();
configurarBotoesDoJogo();