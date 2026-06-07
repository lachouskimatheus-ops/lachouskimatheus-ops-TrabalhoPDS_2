let socket = null;
let cartasSelecionadas = [];
let faseAtual = "CARREGANDO";

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
    } else {
        document.getElementById("vencedor").textContent =
            "Escolha suas cartas e confirme a troca para revelar o resultado.";
    }
}

function atualizarTrocas(estado) {
    document.getElementById("tag-trocas-jogador").textContent =
        `${estado.trocas.jogador} troca(s)`;

    document.getElementById("tag-trocas-computador").textContent =
        `${estado.trocas.computador} troca(s)`;
}

function desenharMao(idElemento, mao, clicavel) {
    const container = document.getElementById(idElemento);
    container.innerHTML = "";

    mao.forEach((carta, indice) => {
        const div = document.createElement("div");
        div.classList.add("carta");

        if (carta.oculta) {
            div.classList.add("oculta");
            div.innerHTML = `
                <div class="verso-carta">
                    <span>♠</span>
                    <span>♦</span>
                </div>
            `;
        } else {
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
        }

        if (clicavel && faseAtual === "ESCOLHENDO_TROCAS") {
            div.classList.add("clicavel");
            div.addEventListener("click", () => alternarSelecao(indice, div));
        }

        container.appendChild(div);
    });
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

function atualizarBotoes() {
    const podeTrocar = faseAtual === "ESCOLHENDO_TROCAS";
    const resultadoRevelado = faseAtual === "RESULTADO";

    document.getElementById("btn-trocar").disabled = !podeTrocar;

    if (podeTrocar) {
        document.getElementById("btn-trocar").textContent = "Confirmar troca";
        document.getElementById("btn-nova-rodada").textContent = "Reiniciar rodada";
    }

    if (resultadoRevelado) {
        document.getElementById("btn-trocar").textContent = "Troca finalizada";
        document.getElementById("btn-nova-rodada").textContent = "Próxima rodada";
    }
}

document.getElementById("btn-trocar").addEventListener("click", () => {
    enviarAcao("TROCAR_CARTAS", {
        indices: cartasSelecionadas
    });
});

document.getElementById("btn-nova-rodada").addEventListener("click", () => {
    enviarAcao("NOVA_RODADA");
});

conectar();