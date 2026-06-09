// ==========================================
// SISTEMA DE ÁUDIO
// ==========================================
const CAMINHO_SONS = "/assets/sons/";
const sonsPreCarregados = {};

function preCarregarSom(nomeArquivo) {
    const audio = new Audio(`${CAMINHO_SONS}${nomeArquivo}`);
    audio.preload = "auto";
    audio.load();
    sonsPreCarregados[nomeArquivo] = audio;
}

function tocarSom(nomeArquivo) {
    const audioOriginal = sonsPreCarregados[nomeArquivo];
    if (!audioOriginal) return;
    const audio = audioOriginal.cloneNode(true);
    audio.currentTime = 0;
    audio.play().catch(() => {});
}

["jogar_carta.ogg", "shuffle.mp3", "victory_6.mp3", "click.mp3"].forEach(preCarregarSom);

// ==========================================
// IDENTIFICAÇÃO DO JOGADOR
// ==========================================
const urlParams = new URLSearchParams(window.location.search);
const idNaUrl = parseInt(urlParams.get('id'));

// Se já tem ?id=X na URL, usa direto. Senão, pede nome e aguarda o servidor atribuir.
let meuId = idNaUrl || null;
let meuNome = null;

// Se já tem id na URL, recupera o nome salvo
if (meuId) {
    meuNome = localStorage.getItem('truco_nome_' + meuId) || ('Jogador ' + meuId);
}

// ==========================================
// INICIALIZAÇÃO
// ==========================================
document.addEventListener("DOMContentLoaded", () => {
    if (!meuId) {
        // Novo jogador: mostra tela de nome antes de conectar
        pedirNome();
    } else {
        // Jogador retornando (tem ?id= na URL): conecta direto
        conectarWebSocket();
    }
});

function pedirNome() {
    // Cria overlay de entrada de nome
    const overlay = document.createElement('div');
    overlay.id = 'tela-nome';
    overlay.style.cssText = `
        position: fixed; inset: 0; background: rgba(0,0,0,0.85);
        display: flex; justify-content: center; align-items: center;
        z-index: 2000; backdrop-filter: blur(8px);
    `;
    overlay.innerHTML = `
        <div style="
            background: #1e293b; border: 2px solid #334155;
            border-radius: 20px; padding: 50px 60px;
            text-align: center; min-width: 340px;
            box-shadow: 0 20px 50px rgba(0,0,0,0.8);
        ">
            <div style="font-size: 22px; color: #f0c040; letter-spacing: 8px; margin-bottom: 12px;">♠ ♥ ♦ ♣</div>
            <h2 style="color: #f8fafc; font-size: 1.8rem; letter-spacing: 4px; margin-bottom: 30px;">TRUCO</h2>
            <p style="color: #9ca3af; margin-bottom: 16px; font-size: 0.95rem; text-transform: uppercase; letter-spacing: 2px;">Seu nome</p>
            <input id="input-nome" type="text" maxlength="20" placeholder="Digite seu nome..."
                style="
                    width: 100%; padding: 14px; background: #0f172a;
                    border: 1.5px solid #334155; border-radius: 10px;
                    color: #f8fafc; font-size: 1.1rem; outline: none;
                    text-align: center; margin-bottom: 20px;
                    transition: border-color 0.2s;
                "
                onfocus="this.style.borderColor='#f0c040'"
                onblur="this.style.borderColor='#334155'"
            />
            <button id="btn-confirmar-nome" onclick="confirmarNome()" style="
                width: 100%; padding: 14px; background: #b45309;
                border: none; border-radius: 12px; color: white;
                font-size: 1rem; font-weight: 700; letter-spacing: 3px;
                text-transform: uppercase; cursor: pointer;
                box-shadow: 0 0 20px rgba(180,83,9,0.3);
                transition: all 0.2s;
            ">Entrar</button>
        </div>
    `;
    document.body.appendChild(overlay);

    // Foca no input e permite confirmar com Enter
    setTimeout(() => {
        const input = document.getElementById('input-nome');
        input.focus();
        input.addEventListener('keydown', e => { if (e.key === 'Enter') confirmarNome(); });
    }, 100);
}

function confirmarNome() {
    const input = document.getElementById('input-nome');
    const nome = input.value.trim();
    if (!nome) {
        input.style.borderColor = '#dc2626';
        input.placeholder = 'Digite um nome!';
        return;
    }
    meuNome = nome;
    document.getElementById('tela-nome').remove();
    conectarWebSocket();
}

// ==========================================
// WEBSOCKET
// ==========================================
let socket = null;

function conectarWebSocket() {
    socket = new WebSocket(`ws://${window.location.host}/ws/truco`);

    socket.onopen = function () {
        console.log("WebSocket Truco conectado.");
        // Se não tem id ainda, envia o nome para o servidor atribuir um id
        if (!meuId) {
            socket.send(JSON.stringify({ acao: "REGISTRAR", nome: meuNome }));
        } else {
            // Reconecta com id já conhecido
            socket.send(JSON.stringify({ acao: "RECONECTAR", jogador_id: meuId, nome: meuNome }));
        }
    };

    socket.onmessage = function (event) {
        const estado = JSON.parse(event.data);

        // Servidor atribuiu um ID para este jogador
        if (estado.meu_id && !meuId) {
            meuId = estado.meu_id;
            localStorage.setItem('truco_nome_' + meuId, meuNome);
            // Atualiza a URL com o id sem recarregar a página
            const novaUrl = window.location.pathname + '?id=' + meuId;
            window.history.replaceState(null, '', novaUrl);
        }

        atualizarInterface(estado);
    };

    socket.onerror = function (e) {
        console.error("Erro WebSocket:", e);
    };
}

// ==========================================
// ATUALIZAÇÃO DA INTERFACE E LOBBY
// ==========================================
function atualizarInterface(dados) {
    window.estadoTrucoAtual = dados;

    const modalLobby = document.getElementById('modal-lobby');
    const lobbyCriacao = document.getElementById('lobby-criacao');
    const lobbyEntrada = document.getElementById('lobby-entrada');
    const lobbyEspera = document.getElementById('lobby-espera');

    const jaEntrou = dados.jogadores && dados.jogadores.find(j => j.id === meuId) !== undefined;
    const salaCheia = dados.jogadores && dados.jogadores.length === dados.max_jogadores;

    if (!salaCheia) {
        modalLobby.classList.remove('modal-oculto');
        lobbyCriacao.classList.add('escondido');
        lobbyEntrada.classList.add('escondido');
        lobbyEspera.classList.add('escondido');

        if (jaEntrou) {
            lobbyEspera.classList.remove('escondido');
            document.getElementById('info-jogadores-espera').innerText =
                `${dados.jogadores.length} de ${dados.max_jogadores} jogadores conectados.`;
        }
        else if (!dados.sala_configurada) {
            lobbyCriacao.classList.remove('escondido');
            atualizarInterfaceLobby();
        }
        else {
            lobbyEntrada.classList.remove('escondido');
            document.getElementById('info-sala').innerText =
                `Truco ${dados.modalidade === 'paulista' ? 'Paulista' : 'Mineiro'} - ${dados.max_jogadores} Jogadores`;

            const btnEq1 = document.getElementById('btn-entrar-eq1');
            const btnEq2 = document.getElementById('btn-entrar-eq2');

            if (dados.max_jogadores === 2) {
                btnEq1.innerText = "Entrar no Jogo";
                btnEq2.style.display = "none";
                btnEq1.disabled = false;
            } else {
                btnEq2.style.display = "inline-block";
                btnEq1.innerText = `Equipe 1 (${dados.vagas_eq1} vagas)`;
                btnEq2.innerText = `Equipe 2 (${dados.vagas_eq2} vagas)`;
                btnEq1.disabled = dados.vagas_eq1 <= 0;
                btnEq2.disabled = dados.vagas_eq2 <= 0;
                btnEq1.style.opacity = dados.vagas_eq1 <= 0 ? "0.5" : "1";
                btnEq2.style.opacity = dados.vagas_eq2 <= 0 ? "0.5" : "1";
            }
        }
        return;
    } else {
        modalLobby.classList.add('modal-oculto');
    }

    // --- RENDERIZAÇÃO DA MESA ---
    const estadoAnterior = window.estadoTrucoAnterior || {};
    const eu = dados.jogadores ? dados.jogadores.find(j => j.id === meuId) : null;
    const euAnterior = estadoAnterior.jogadores ? estadoAnterior.jogadores.find(j => j.id === meuId) : null;
    const novaDistribuicao = (!euAnterior && eu && eu.mao.length > 0)
        || (euAnterior && eu && eu.mao.length > euAnterior.mao.length);
    window.estadoTrucoAnterior = dados;

    // Placar — no 1v1 usa nomes, no 2v2 usa "Equipe X"
    const modo1v1 = dados.max_jogadores === 2;
    if (modo1v1 && dados.jogadores && dados.jogadores.length === 2) {
        const jog1 = dados.jogadores.find(j => j.equipe === 1);
        const jog2 = dados.jogadores.find(j => j.equipe === 2);
        document.querySelector('.placar-equipe:first-child .placar-label').innerText = jog1 ? jog1.nome : 'Jogador 1';
        document.querySelector('.placar-equipe:last-child .placar-label').innerText  = jog2 ? jog2.nome : 'Jogador 2';
    } else {
        document.querySelector('.placar-equipe:first-child .placar-label').innerText = 'Equipe 1';
        document.querySelector('.placar-equipe:last-child .placar-label').innerText  = 'Equipe 2';
    }
    document.getElementById('pontos-eq1').innerText = dados.pontos_equipe1 ?? 0;
    document.getElementById('pontos-eq2').innerText = dados.pontos_equipe2 ?? 0;

    // Valor da mão e queda
    document.getElementById('valor-mao').innerText = dados.valor_mao ?? 1;
    const quedaTextos = ["", "1ª Queda", "2ª Queda", "3ª Queda"];
    document.getElementById('queda-atual').innerText = quedaTextos[dados.queda_atual] || "";

    // Indica regra dos 11
    const valorMaoEl = document.getElementById('valor-mao');
    if (dados.mao_travada) {
        valorMaoEl.title = "Regra dos 11: mão vale 3 pontos, sem truco";
        valorMaoEl.style.color = '#f87171';
    } else {
        valorMaoEl.title = '';
        valorMaoEl.style.color = '';
    }

    // Vira
    const viraDiv = document.getElementById('carta-vira');
    if (dados.vira && dados.vira.valor !== undefined) {
        const textoValor = traduzirValor(dados.vira.valor);
        const simbolo = obterSimbolo(dados.vira.naipe);
        viraDiv.innerHTML = `<span>${textoValor}</span>${gerarFigura(textoValor, dados.vira.naipe)}`;
        viraDiv.className = `carta ${dados.vira.naipe}`;
        viraDiv.setAttribute('data-naipe-simbolo', simbolo);
    } else {
        viraDiv.innerHTML = "?";
        viraDiv.className = "carta";
    }

    // Cartas na mesa
    const mesaDiv = document.getElementById('cartas-na-mesa');
    mesaDiv.innerHTML = '';
    if (dados.cartas_na_mesa) {
        dados.cartas_na_mesa.forEach((carta, i) => {
            const el = document.createElement('div');
            const textoValor = traduzirValor(carta.valor);
            const simbolo = obterSimbolo(carta.naipe);
            el.className = `carta ${carta.naipe}`;
            el.innerHTML = `<span>${textoValor}</span>${gerarFigura(textoValor, carta.naipe)}`;
            el.setAttribute('data-naipe-simbolo', simbolo);
            const rotacoes = [-4, 3, -2, 5];
            el.style.setProperty('--rotacao-mesa', `${rotacoes[i % rotacoes.length]}deg`);
            mesaDiv.appendChild(el);
        });
    }

    // Oponentes
    ['cadeira-esquerda', 'cadeira-topo', 'cadeira-direita'].forEach(id => {
        document.getElementById(id).innerHTML = '';
    });

    if (dados.jogadores) {
        const total = dados.jogadores.length;
        dados.jogadores.forEach(j => {
            if (j.id === meuId) return;

            const pos = (j.id - meuId + total) % total;
            let idCadeira = '';
            if (total === 4) {
                if (pos === 1) idCadeira = 'cadeira-direita';
                else if (pos === 2) idCadeira = 'cadeira-topo';
                else if (pos === 3) idCadeira = 'cadeira-esquerda';
            } else if (total === 2) {
                idCadeira = 'cadeira-topo';
            }
            if (!idCadeira) return;

            const equipe = j.equipe || (j.id % 2 === 0 ? 2 : 1);
            const suaVez = dados.jogador_da_vez === j.id;
            const maoHtml = Array.from({ length: j.cartas_na_mao || 0 })
                .map(() => '<div class="carta-verso"></div>').join('');
            const tagEquipe = modo1v1 ? '' : `<span class="tag-equipe tag-equipe-${equipe}">Equipe ${equipe}</span>`;

            document.getElementById(idCadeira).innerHTML = `
                <div class="perfil-jogador ${suaVez ? 'sua-vez' : ''}">
                    <h3>${j.nome || 'Jogador ' + j.id}</h3>
                    ${tagEquipe}
                </div>
                <div class="mao-oponente">${maoHtml}</div>
            `;
        });

        // Minha mão
        if (eu) {
            document.getElementById('nome-local').innerText = eu.nome || meuNome;
            // No 1v1 oculta a tag de equipe
            const dadosLocaisEl = document.querySelector('.dados-local');
            if (dadosLocaisEl) {
                dadosLocaisEl.style.display = modo1v1 ? 'none' : '';
            }
            document.getElementById('local-equipe').innerText = eu.equipe;
            document.getElementById('local-quedas').innerText =
                eu.equipe === 1 ? (dados.vitoriasEq1 || 0) : (dados.vitoriasEq2 || 0);

            const minhaMaoDiv = document.getElementById('minha-mao');
            minhaMaoDiv.innerHTML = '';
            const ehMinhaVez = dados.jogador_da_vez === meuId;
            const meio = (eu.mao.length - 1) / 2;

            eu.mao.forEach((carta, i) => {
                const el = document.createElement('div');
                const textoValor = traduzirValor(carta.valor);
                const simbolo = obterSimbolo(carta.naipe);
                const angulo = (i - meio) * 6;
                const transY = Math.abs(i - meio) * 4;
                el.style.setProperty('--rotacao', `${angulo}deg`);
                el.style.setProperty('--transY', `${transY}px`);
                el.className = `carta ${carta.naipe}${carta.manilha ? ' manilha' : ''}`;
                el.innerHTML = `<span>${textoValor}</span>${gerarFigura(textoValor, carta.naipe)}`;
                el.setAttribute('data-naipe-simbolo', simbolo);

                if (ehMinhaVez && !dados.aguardando_resposta_truco) {
                    el.onmousedown = () => jogarCarta(i);
                } else {
                    el.classList.add('bloqueada');
                }
                minhaMaoDiv.appendChild(el);
            });
        }
    }

    // Painel de ações
    atualizarPainelAcoes(dados);

    // Eventos
    if (dados.evento) tratarEvento(dados.evento, dados);

    // Animação de distribuição
    if (novaDistribuicao) animarDistribuicao(dados);
}

function atualizarPainelAcoes(dados) {
    const painel = document.getElementById('painel-acoes');
    const titulo = document.getElementById('titulo-acao');
    const botoes = document.getElementById('botoes-acao');
    const ehMinhaVez = dados.jogador_da_vez === meuId;
    botoes.innerHTML = '';

    const eu = dados.jogadores ? dados.jogadores.find(j => j.id === meuId) : null;
    const minhaEquipe = eu ? eu.equipe : 0;

    if (dados.aguardando_resposta_truco && dados.equipe_respondendo === minhaEquipe) {
        titulo.innerText = `Pedido de ${dados.nome_nivel_truco}!`;
        painel.classList.remove('escondido');
        painel.style.display = '';

        const btnAceitar = criarBotao('Aceitar', 'aceitar', () => responderTruco('ACEITAR'));
        const btnRecusar = criarBotao('Recusar', 'recusar', () => responderTruco('RECUSAR'));
        botoes.appendChild(btnAceitar);
        botoes.appendChild(btnRecusar);

        if (dados.nivel_truco < 4) {
            const nomes = ['', 'Truco', 'Seis', 'Nove', 'Doze'];
            const btnAumentar = criarBotao(nomes[dados.nivel_truco + 1] + '!', 'aumentar', () => responderTruco('AUMENTAR'));
            botoes.appendChild(btnAumentar);
        }
    } else if (ehMinhaVez && !dados.aguardando_resposta_truco && dados.nivel_truco < 4 && !dados.mao_travada) {
        titulo.innerText = 'Sua vez:';
        painel.classList.remove('escondido');
        painel.style.display = '';
        const nomes = ['', 'Truco', 'Seis', 'Nove', 'Doze'];
        const btnTruco = criarBotao(nomes[dados.nivel_truco + 1] + '!', 'truco', () => pedirTruco());
        botoes.appendChild(btnTruco);
    } else {
        painel.classList.add('escondido');
    }
}

function criarBotao(texto, classe, acao) {
    const btn = document.createElement('button');
    btn.className = `btn-acao ${classe}`;
    btn.innerText = texto;
    btn.onmousedown = () => { tocarSom('click.mp3'); acao(); };
    return btn;
}

function nomeEquipe(dados, equipe) {
    // No 1v1 usa o nome do jogador, no 2v2 usa "Equipe X"
    if (dados.max_jogadores === 2 && dados.jogadores) {
        const jog = dados.jogadores.find(j => j.equipe === equipe);
        return jog ? jog.nome : ('Jogador ' + equipe);
    }
    return 'Equipe ' + equipe;
}

function tratarEvento(evento, dados) {
    switch (evento) {
        case 'TRUCO_PEDIDO': {
            // Mostra o valor correto: o que está sendo pedido agora
            const valoresPedido = { 'Truco': 3, 'Seis': 6, 'Nove': 9, 'Doze': 12 };
            const valorPedido = valoresPedido[dados.nome_nivel_truco] || dados.valor_se_aceito;
            mostrarModal(`<h2 style="color:#f0c040;">🃏 ${dados.nome_pedidor || 'Alguém'} pediu ${dados.nome_nivel_truco}!</h2><p>Se aceito, a mão passa a valer ${valorPedido} ponto(s)</p>`, 2000);
            tocarSom('click.mp3');
            break;
        }
        case 'TRUCO_ACEITO':
            mostrarModal(`<h2 style="color:#4ade80;">✅ ${dados.nome_nivel_truco} aceito!</h2><p>Mão vale ${dados.valor_mao} ponto(s)</p>`, 2000);
            break;
        case 'TRUCO_RECUSADO':
            mostrarModal(`<h2 style="color:#f87171;">❌ ${dados.nome_nivel_truco} recusado!</h2><p>${nomeEquipe(dados, dados.equipe_vencedora)} ganha ${dados.pontos_ganhos} ponto(s)</p>`, 2500);
            break;
        case 'FIM_QUEDA':
            if (dados.vencedor_queda === 0) {
                mostrarModal(`<h2 style="color:#60a5fa;">🤝 Queda empatou!</h2>`, 1800);
            } else {
                mostrarModal(`<h2 style="color:#4ade80;">⚔️ ${nomeEquipe(dados, dados.vencedor_queda)} venceu a queda!</h2>`, 1800);
            }
            tocarSom('jogar_carta.ogg');
            break;
        case 'FIM_MAO':
            if (dados.vencedor_mao === 0) {
                mostrarModal(`<h2 style="color:#9ca3af;">🤝 Mão empatada! Ninguém pontua.</h2>`, 2500);
            } else {
                mostrarModal(`<h2 style="color:#f0c040;">🏆 ${nomeEquipe(dados, dados.vencedor_mao)} venceu a mão! +${dados.pontos_ganhos}pt</h2>`, 2500);
                tocarSom('victory_6.mp3');
            }
            break;
        case 'FIM_PARTIDA':
            mostrarModal(`
                <h2 style="color:#f0c040;">🥇 ${nomeEquipe(dados, dados.equipe_vencedora)} VENCEU!</h2>
                <p>Placar final: ${dados.pontos_equipe1} × ${dados.pontos_equipe2}</p>
                <p style="margin-top:20px;"><a href="/pages/menu.html" style="color:#60a5fa;">Voltar ao Menu</a></p>
            `, 0);
            tocarSom('victory_6.mp3');
            break;
    }
}

// ==========================================
// AÇÕES DO JOGADOR
// ==========================================
function jogarCarta(indice) {
    tocarSom('jogar_carta.ogg');
    socket.send(JSON.stringify({ acao: "JOGAR_CARTA", jogador_id: meuId, indice: indice }));
}

function pedirTruco() {
    socket.send(JSON.stringify({ acao: "PEDIR_TRUCO", jogador_id: meuId }));
}

function responderTruco(resposta) {
    socket.send(JSON.stringify({ acao: "RESPONDER_TRUCO", jogador_id: meuId, resposta: resposta }));
}

// ==========================================
// AÇÕES DO LOBBY
// ==========================================
function atualizarInterfaceLobby() {
    const qtdJogadores = parseInt(document.getElementById('select-jogadores').value);
    const containerEquipe = document.getElementById('container-equipe-criacao');
    if (qtdJogadores === 2) {
        containerEquipe.style.display = "none";
    } else {
        containerEquipe.style.display = "flex";
    }
}

function criarSala() {
    tocarSom('click.mp3');
    const modalidade = document.getElementById('select-modalidade').value;
    const maxJog = parseInt(document.getElementById('select-jogadores').value);
    const equipe = maxJog === 2 ? 1 : parseInt(document.getElementById('select-equipe-criacao').value);

    socket.send(JSON.stringify({
        acao: "CRIAR_SALA",
        jogador_id: meuId,
        nome: meuNome,
        max_jogadores: maxJog,
        modalidade: modalidade,
        equipe: equipe
    }));
}

function entrarSala(equipeEscolhida) {
    tocarSom('click.mp3');
    const dados = window.estadoTrucoAtual;

    if (dados.max_jogadores === 2) {
        equipeEscolhida = dados.vagas_eq1 > 0 ? 1 : 2;
    }

    socket.send(JSON.stringify({
        acao: "ENTRAR",
        jogador_id: meuId,
        nome: meuNome,
        equipe: equipeEscolhida
    }));
}

// ==========================================
// FUNÇÕES AUXILIARES
// ==========================================
function traduzirValor(valor) {
    const mapa = { 1: "A", 11: "J", 12: "Q", 13: "K" };
    return mapa[valor] || valor;
}

function obterSimbolo(naipe) {
    const simbolos = { paus: "♣", copas: "♥", espadas: "♠", ouros: "♦" };
    return simbolos[naipe] || "";
}

function gerarFigura(textoValor, naipe) {
    const figuras = { K: "rei.png", Q: "rainha.png", J: "valete.png" };
    const arquivo = figuras[textoValor];
    if (!arquivo) return "";
    const vermelho = (naipe === "copas" || naipe === "ouros") ? " filtro-vermelho" : "";
    return `<img src="/assets/cartas/${arquivo}" class="figura-centro${vermelho}" alt="${textoValor}">`;
}

function mostrarModal(htmlContent, tempoMs) {
    const modal = document.getElementById('modal-notificacao');
    const texto = document.getElementById('modal-texto');
    texto.innerHTML = htmlContent;
    modal.classList.remove('modal-oculto');
    if (window.modalTimer) clearTimeout(window.modalTimer);
    if (tempoMs > 0) {
        window.modalTimer = setTimeout(() => modal.classList.add('modal-oculto'), tempoMs);
    }
}

function animarDistribuicao(dados) {
    tocarSom('shuffle.mp3');
    const baralhoEl = document.querySelector('.centro-da-mesa');
    if (!baralhoEl) return;
    const rect = baralhoEl.getBoundingClientRect();
    const origemX = rect.left + rect.width / 2;
    const origemY = rect.top + rect.height / 2;
    let delay = 0;

    if (!dados.jogadores) return;
    dados.jogadores.forEach(j => {
        let idAlvo = 'cadeira-base';
        if (j.id !== meuId) {
            const total = dados.jogadores.length;
            const pos = (j.id - meuId + total) % total;
            if (total === 4) {
                if (pos === 1) idAlvo = 'cadeira-direita';
                else if (pos === 2) idAlvo = 'cadeira-topo';
                else if (pos === 3) idAlvo = 'cadeira-esquerda';
            } else if (total === 2) {
                idAlvo = 'cadeira-topo';
            }
        }
        const assento = document.getElementById(idAlvo);
        if (!assento) return;
        const rectAlvo = assento.getBoundingClientRect();
        const destX = rectAlvo.left + rectAlvo.width / 2;
        const destY = rectAlvo.top + rectAlvo.height / 2;
        const qtd = j.cartas_na_mao || (j.mao ? j.mao.length : 3);

        for (let c = 0; c < qtd; c++) {
            setTimeout(() => {
                const cartaVoadora = document.createElement('div');
                cartaVoadora.className = 'carta-verso carta-animada';
                cartaVoadora.style.left = origemX + 'px';
                cartaVoadora.style.top = origemY + 'px';
                cartaVoadora.style.width = '40px';
                cartaVoadora.style.height = '60px';
                cartaVoadora.style.transform = 'translate(-50%, -50%) scale(1)';
                document.body.appendChild(cartaVoadora);
                requestAnimationFrame(() => {
                    cartaVoadora.style.left = destX + 'px';
                    cartaVoadora.style.top = destY + 'px';
                    cartaVoadora.style.transform = 'translate(-50%, -50%) scale(0.5) rotate(720deg)';
                    cartaVoadora.style.opacity = '0';
                });
                setTimeout(() => cartaVoadora.remove(), 600);
            }, delay);
            delay += 80;
        }
    });
}