const sons = {
    jogar: new Audio("/assets/sons/jogar_carta.ogg"),
    shuffle: new Audio("/assets/sons/shuffle.mp3"),
    vitoria: new Audio("/assets/sons/victory_6.mp3"),
    click: new Audio("/assets/sons/click.mp3"),
    selecionar: new Audio("/assets/sons/selecionar.mp3")
};

function tocarSom(nome) {
    if (!sons[nome]) return;
    sons[nome].currentTime = 0;
    sons[nome].play().catch(erro => console.log("Erro de áudio:", erro));
}

let socket = null;
let meuId = null;
let cartaArrastada = null;
let ordemLocalMao = [];
let cartaSelecionadaIndice = null;
let estadoAtualGlobal = null;
let jogoJaAcabou = false;

const minhaMao = document.getElementById("minha-mao");
const monteCompra = document.getElementById("monte-compra");
const mesaDescarte = document.getElementById("mesa-descarte");
const cartaVira = document.getElementById("carta-vira");
const textoCoringa = document.getElementById("carta-coringa");
const qtdCartas = document.getElementById("qtd-cartas");
const jogadorAtualTexto = document.getElementById("jogador-atual");
const btnBater = document.getElementById("btn-bater");
const btnOrganizar = document.getElementById("btn-organizar");

iniciarWebSocket();

function iniciarWebSocket() {
    const parametros = new URLSearchParams(window.location.search);
    const sala = parametros.get("sala") || "global";
    const protocolo = location.protocol === "https:" ? "wss" : "ws";
    const chaveToken = `pife_token_${sala}`;

    let token = sessionStorage.getItem(chaveToken);

    if (!token) {
        token = typeof crypto.randomUUID === "function"
            ? crypto.randomUUID()
            : `${Date.now()}-${Math.random().toString(36).slice(2)}`;

        sessionStorage.setItem(chaveToken, token);
    }

    socket = new WebSocket(`${protocolo}://${location.host}/ws/pife`);

    socket.onopen = () => {
        socket.send(JSON.stringify({
            tipo: "entrar_sala",
            sala,
            token
        }));
    };

    socket.onmessage = event => {
        let msg;

        try {
            msg = JSON.parse(event.data);
        } catch (erro) {
            console.error("Mensagem inválida recebida:", event.data);
            return;
        }

        if (msg.erro) {
            mostrarModal(msg.erro);
            return;
        }

        if (
            msg.tipo === "entrada_confirmada" ||
            msg.tipo === "reconexao_confirmada"
        ) {
            meuId = msg.idJogador;

            mostrarModal(
                msg.tipo === "reconexao_confirmada"
                    ? `Jogador ${meuId + 1} reconectado`
                    : `Jogador ${meuId + 1} conectado`
            );

            return;
        }

        if (!Array.isArray(msg.minha_mao)) return;

        if (Number.isInteger(msg.meu_id)) {
            meuId = msg.meu_id;
        }

        renderizarEstado(msg);
    };

    socket.onerror = erro => {
        console.error("Erro no WebSocket:", erro);
    };

    socket.onclose = () => {
        mostrarModal("Conexão encerrada");
    };
}

function enviarAcao(acao, dados = {}) {
    if (!socket || socket.readyState !== WebSocket.OPEN) {
        mostrarModal("Sem conexão com o servidor");
        return;
    }

    socket.send(JSON.stringify({
        tipo: "acao_jogo",
        acao,
        ...dados
    }));
}

function renderizarEstado(estado) {
    estadoAtualGlobal = estado;

    renderizarMao(estado.minha_mao);
    renderizarOponentes(estado);
    renderizarMesa(estado.mesa || []);
    renderizarVira(estado.vira);

    if (qtdCartas) {
        qtdCartas.textContent = estado.minha_mao.length;
    }

    if (jogadorAtualTexto) {
        jogadorAtualTexto.textContent = Number.isInteger(estado.jogador_atual)
            ? `Jogador ${estado.jogador_atual + 1}`
            : "Aguardando jogadores";
    }

    atualizarBotoesEAreas(estado);
    verificarVitoria(estado);

    cartaSelecionadaIndice = null;
}

function renderizarOponentes(estado) {
    const cadeiraDireita = document.getElementById("cadeira-direita");
    const cadeiraTopo = document.getElementById("cadeira-topo");
    const cadeiraEsquerda = document.getElementById("cadeira-esquerda");

    if (cadeiraDireita) cadeiraDireita.innerHTML = "";
    if (cadeiraTopo) cadeiraTopo.innerHTML = "";
    if (cadeiraEsquerda) cadeiraEsquerda.innerHTML = "";

    let listaJogadores = estado.jogadores;

    if (!Array.isArray(listaJogadores)) {
        const parametros = new URLSearchParams(window.location.search);
        const totalParametro = Number(parametros.get("jogadores") || 2);
        const total = estado.total_jogadores || estado.numero_jogadores || totalParametro;

        listaJogadores = [];

        for (let i = 0; i < total; i++) {
            listaJogadores.push({
                id: i,
                nome: `Jogador ${i + 1}`,
                quantidade_cartas:
                    estado.jogador_atual === i &&
                    estado.fase_turno === "AguardandoDescarte"
                        ? 10
                        : 9
            });
        }
    }

    const totalJogadores = listaJogadores.length;

    listaJogadores.forEach(jogador => {
        if (jogador.id === meuId) return;

        const posicaoRelativa =
            (jogador.id - meuId + totalJogadores) % totalJogadores;

        let idCadeira = "";

        if (totalJogadores === 4) {
            if (posicaoRelativa === 1) idCadeira = "cadeira-direita";
            else if (posicaoRelativa === 2) idCadeira = "cadeira-topo";
            else if (posicaoRelativa === 3) idCadeira = "cadeira-esquerda";
        } else if (totalJogadores === 3) {
            if (posicaoRelativa === 1) idCadeira = "cadeira-direita";
            else if (posicaoRelativa === 2) idCadeira = "cadeira-esquerda";
        } else {
            idCadeira = "cadeira-topo";
        }

        if (!idCadeira) return;

        const cadeira = document.getElementById(idCadeira);

        if (!cadeira) return;

        const quantidadeCartas =
            jogador.quantidade_cartas ??
            jogador.qtd_cartas ??
            jogador.cartas_na_mao ??
            9;

        const turnoAtual = estado.jogador_atual === jogador.id;
        const nome = jogador.nome || `Jogador ${jogador.id + 1}`;

        let htmlCartas = '<div class="mao-oponente">';

        for (let i = 0; i < quantidadeCartas; i++) {
            htmlCartas += '<div class="carta-verso"></div>';
        }

        htmlCartas += "</div>";

        cadeira.innerHTML = `
            <h2 class="${turnoAtual ? "jogador-em-turno" : ""}">${nome}</h2>
            ${htmlCartas}
        `;
    });
}

function renderizarMao(cartas) {
    if (!minhaMao) return;

    minhaMao.innerHTML = "";

    const lista = cartas.map((carta, indice) => ({
        ...carta,
        idx: indice,
        id: `${carta.valor}-${carta.naipe}`
    }));

    lista.sort((a, b) => {
        const posicaoA = ordemLocalMao.indexOf(a.id);
        const posicaoB = ordemLocalMao.indexOf(b.id);

        return (posicaoA === -1 ? 999 : posicaoA) -
            (posicaoB === -1 ? 999 : posicaoB);
    });

    lista.forEach(item => {
        const elemento = criarCarta(item);

        elemento.dataset.idx = item.idx;
        elemento.dataset.id = item.id;
        elemento.draggable = true;

        elemento.ondragstart = () => {
            cartaArrastada = elemento;
            cartaSelecionadaIndice = item.idx;
            elemento.classList.add("dragging");
            tocarSom("selecionar");
        };

        elemento.ondragend = () => {
            elemento.classList.remove("dragging");
            cartaArrastada = null;

            ordemLocalMao = [
                ...minhaMao.querySelectorAll(".carta")
            ].map(carta => carta.dataset.id);
        };

        elemento.onclick = () => {
            minhaMao
                .querySelectorAll(".carta")
                .forEach(carta => carta.classList.remove("selecionada"));

            elemento.classList.add("selecionada");
            cartaSelecionadaIndice = item.idx;

            tocarSom("selecionar");
        };

        minhaMao.appendChild(elemento);
    });
}

if (minhaMao) {
    minhaMao.ondragover = event => {
        event.preventDefault();

        if (!cartaArrastada) return;

        const cartas = [
            ...minhaMao.querySelectorAll(".carta:not(.dragging)")
        ];

        const cartaPosterior = cartas.find(carta => {
            const retangulo = carta.getBoundingClientRect();
            return event.clientX <= retangulo.left + retangulo.width / 2;
        });

        if (cartaPosterior) {
            minhaMao.insertBefore(cartaArrastada, cartaPosterior);
        } else {
            minhaMao.appendChild(cartaArrastada);
        }
    };
}

function renderizarMesa(cartas) {
    if (!mesaDescarte) return;

    mesaDescarte.innerHTML = "";

    if (cartas.length === 0) {
        mesaDescarte.innerHTML =
            '<div class="carta carta-vazia">Descarte</div>';

        return;
    }

    mesaDescarte.appendChild(criarCarta(cartas[cartas.length - 1]));
}

function renderizarVira(vira) {
    if (!cartaVira) return;

    cartaVira.innerHTML = "";

    if (!vira) {
        if (textoCoringa) textoCoringa.textContent = "-";
        return;
    }

    cartaVira.appendChild(criarCarta(vira));

    const valorVira = Number(vira.valor);
    const valorCoringa = valorVira === 13 ? 1 : valorVira + 1;

    if (textoCoringa) {
        textoCoringa.textContent = cartaParaTexto({
            valor: valorCoringa,
            naipe: vira.naipe
        });
    }
}

function criarCarta(carta) {
    const imagem = document.createElement("img");

    imagem.className = "carta";
    imagem.src = caminhoImagemCarta(carta);
    imagem.alt = cartaParaTexto(carta);
    imagem.draggable = false;

    imagem.onerror = () => {
        console.error("Imagem da carta não encontrada:", imagem.src);
    };

    return imagem;
}

function caminhoImagemCarta(carta) {
    const naipes = {
        0: "clubs",
        1: "hearts",
        2: "spades",
        3: "diamonds"
    };

    const valores = {
        1: "ace",
        2: "02",
        3: "03",
        4: "04",
        5: "05",
        6: "06",
        7: "07",
        8: "08",
        9: "09",
        10: "10",
        11: "jack",
        12: "queen",
        13: "king"
    };

    const naipe = naipes[Number(carta.naipe)];
    const valor = valores[Number(carta.valor)];

    return `/assets/cartas/${naipe}_${valor}.png`;
}

function cartaParaTexto(carta) {
    const valores = {
        1: "A",
        11: "J",
        12: "Q",
        13: "K"
    };

    const naipes = {
        0: "♣",
        1: "♥",
        2: "♠",
        3: "♦"
    };

    return `${valores[carta.valor] || carta.valor}${naipes[carta.naipe] || ""}`;
}

function atualizarBotoesEAreas(estado) {
    if (btnBater) {
        btnBater.disabled = !estado.pode_bater;
    }

    if (btnOrganizar) {
        btnOrganizar.disabled = estado.jogo_finalizado;
    }

    const statusBox = document.querySelector(".status-box");

    if (statusBox) {
        const minhaVez =
            estado.jogador_atual === meuId &&
            !estado.jogo_finalizado;

        statusBox.classList.toggle("sua-vez", minhaVez);
    }

    if (monteCompra) {
        monteCompra.classList.toggle(
            "ativo",
            Boolean(estado.pode_comprar_baralho)
        );
    }

    if (mesaDescarte) {
        mesaDescarte.classList.toggle(
            "ativo",
            Boolean(estado.pode_comprar_mesa)
        );
    }
}

if (monteCompra) {
    monteCompra.onclick = () => {
        if (!estadoAtualGlobal?.pode_comprar_baralho) return;

        tocarSom("selecionar");
        enviarAcao("COMPRAR_BARALHO");
    };
}

if (mesaDescarte) {
    mesaDescarte.onclick = () => {
        if (estadoAtualGlobal?.pode_comprar_mesa) {
            tocarSom("selecionar");
            enviarAcao("COMPRAR_MESA");
            return;
        }

        if (
            cartaSelecionadaIndice !== null &&
            estadoAtualGlobal?.pode_descartar
        ) {
            tocarSom("jogar");

            enviarAcao("DESCARTAR", {
                indice: cartaSelecionadaIndice
            });

            cartaSelecionadaIndice = null;
        }
    };

    mesaDescarte.addEventListener("dragover", event => {
        event.preventDefault();

        if (
            cartaSelecionadaIndice !== null &&
            estadoAtualGlobal?.pode_descartar
        ) {
            mesaDescarte.classList.add("drag-over");
        }
    });

    mesaDescarte.addEventListener("dragleave", () => {
        mesaDescarte.classList.remove("drag-over");
    });

    mesaDescarte.addEventListener("drop", event => {
        event.preventDefault();
        mesaDescarte.classList.remove("drag-over");

        if (
            cartaSelecionadaIndice === null ||
            !estadoAtualGlobal?.pode_descartar
        ) {
            return;
        }

        tocarSom("jogar");

        enviarAcao("DESCARTAR", {
            indice: cartaSelecionadaIndice
        });

        cartaSelecionadaIndice = null;
    });
}

if (btnBater) {
    btnBater.onclick = () => {
        if (!estadoAtualGlobal?.pode_bater) return;

        tocarSom("click");
        enviarAcao("BATER");
    };
}

if (btnOrganizar) {
    btnOrganizar.onclick = () => {
        if (estadoAtualGlobal?.jogo_finalizado) return;

        tocarSom("click");
        ordemLocalMao = [];
        enviarAcao("ORGANIZAR");
    };
}

function verificarVitoria(estado) {
    if (estado.jogo_finalizado && !jogoJaAcabou) {
        jogoJaAcabou = true;

        if (estado.vencedor === meuId) {
            mostrarModal("Você bateu! 🎉");
            tocarSom("vitoria");
        } else if (Number.isInteger(estado.vencedor)) {
            mostrarModal(`Jogador ${estado.vencedor + 1} bateu!`);
        } else {
            mostrarModal("Partida finalizada");
        }
    }

    if (!estado.jogo_finalizado) {
        jogoJaAcabou = false;
    }
}

function mostrarModal(texto) {
    const modal = document.getElementById("modal-notificacao");
    const modalTexto = document.getElementById("modal-texto");

    if (!modal || !modalTexto) {
        console.log(texto);
        return;
    }

    modalTexto.textContent = texto;
    modal.classList.remove("modal-oculto");

    clearTimeout(mostrarModal.timeout);

    mostrarModal.timeout = setTimeout(() => {
        modal.classList.add("modal-oculto");
    }, 2000);
}