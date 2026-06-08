let socket = null;

let cartasSelecionadas = [];
let faseAtual = "MENU";

let modoSelecionado = "computador";
let quantidadeJogadoresSelecionada = 1;

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
        avisoModo.textContent = "Modo selecionado: Treino individual. Será integrado na próxima etapa.";
        return;
    }

    avisoModo.textContent =
        `Modo selecionado: ${quantidadeJogadoresSelecionada} jogadores. Multiplayer será integrado na próxima etapa.`;
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
        "Escolha até 3 cartas para trocar e enfrente o computador.";

    document.getElementById("titulo-adversario").textContent = "Computador";
    document.getElementById("descricao-adversario").textContent =
        "A mão fica oculta até o resultado.";

    faseAtual = "CARREGANDO";
}

function voltarParaMenuPoker() {
    document.getElementById("tela-jogo").classList.add("escondida");
    document.getElementById("tela-menu").classList.remove("escondida");

    faseAtual = "MENU";
    cartasSelecionadas = [];

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
        document.getElementById("status").textContent = "Conectado ao servidor do Poker.";
        enviarAcao("OBTER_ESTADO_ATUAL");
    };

    socket.onmessage = (event) => {
        const estado = JSON.parse(event.data);
        atualizarTela(estado);
    };

    socket.onclose = () => {
        document.getElementById("status").textContent = "Conexão encerrada com o servidor.";
    };

    socket.onerror = () => {
        document.getElementById("status").textContent = "Erro na conexão com o servidor.";
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

    atualizarPlacar(estado);
    atualizarMensagens(estado);
    atualizarTrocas(estado);

    desenharMao("mao-jogador", estado.jogador.mao, true);
    desenharMao("mao-computador", estado.computador.mao, false);

    document.getElementById("jogada-jogador").textContent = estado.jogador.jogada;
    document.getElementById("jogada-computador").textContent = estado.computador.jogada;

    atualizarContadorSelecionadas();
    atualizarBotoes();
    atualizarClasseDaFase();
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
            document.getElementById("vencedor").textContent = "Você venceu esta rodada!";
        } else if (estado.vencedor === "Computador") {
            document.getElementById("vencedor").textContent = "O computador venceu esta rodada.";
        } else {
            document.getElementById("vencedor").textContent = "Empate na rodada.";
        }

        return;
    }

    document.getElementById("vencedor").textContent =
        "Escolha suas cartas e confirme a troca para revelar o resultado.";
}

function atualizarTrocas(estado) {
    document.getElementById("tag-trocas-jogador").textContent =
        `${estado.trocas.jogador} troca(s)`;

    document.getElementById("tag-trocas-computador").textContent =
        `${estado.trocas.computador} troca(s)`;
}

function atualizarClasseDaFase() {
    document.body.classList.remove("fase-escolha", "fase-resultado");

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
}

function atualizarContadorSelecionadas() {
    document.getElementById("selecionadas").textContent =
        `Cartas selecionadas: ${cartasSelecionadas.length}/3`;
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
        enviarAcao("NOVA_RODADA");
    });
}

function confirmarTrocaComAnimacao() {
    if (faseAtual !== "ESCOLHENDO_TROCAS") {
        return;
    }

    const cartas = document.querySelectorAll("#mao-jogador .carta");

    cartasSelecionadas.forEach((indice) => {
        if (cartas[indice]) {
            cartas[indice].classList.add("trocando");
        }
    });

    document.getElementById("status").textContent =
        cartasSelecionadas.length === 0
            ? "Nenhuma carta trocada. Revelando resultado..."
            : "Trocando cartas selecionadas...";

    setTimeout(() => {
        enviarAcao("TROCAR_CARTAS", {
            indices: cartasSelecionadas
        });
    }, 420);
}

// ===============================
// INICIALIZAÇÃO
// ===============================

configurarMenuInicial();
configurarBotoesDoJogo();