// ==========================================
// SISTEMA DE ÁUDIO DE ALTA PERFORMANCE
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
    if (!audioOriginal) {
        console.warn(`Som não foi pré-carregado: ${nomeArquivo}`);
        return;
    }
    const audio = audioOriginal.cloneNode(true);
    audio.currentTime = 0;
    audio.play().catch((erro) => {
        console.log(`Não foi possível reproduzir o som ${nomeArquivo}.`, erro);
    });
}

[
    "jogar_carta.ogg",
    "shuffle.mp3",
    "funny_82hiegE.mp3",
    "knife-cut.mp3",
    "victory_6.mp3",
    "click.mp3"
].forEach(preCarregarSom);

// ==========================================
// VARIÁVEIS GERAIS
// ==========================================
const urlParams = new URLSearchParams(window.location.search);
let meuId = parseInt(urlParams.get('id')) || 0;
const salaAtual = urlParams.get('sala') || 'global';
let meuNome = 'Anônimo'; 
let socket = null;

let configJogo = {
    verso: localStorage.getItem('pref_verso') || 'verso1',
    baralho: localStorage.getItem('pref_baralho') || 'padrao',
    avatar: parseInt(localStorage.getItem('pref_avatar')) || 1
};

// ==========================================
// EVENTOS DE INICIALIZAÇÃO E MODAL DE NOME
// ==========================================
document.addEventListener("DOMContentLoaded", () => {
    aplicarConfiguracoes();

    const modalEntrada = document.getElementById("modal-entrada");
    const inputNome = document.getElementById("input-nome");
    const btnEntrar = document.getElementById("btn-entrar-jogo");
    const codigoSalaVisor = document.getElementById("codigo-sala-entrada");

    if (codigoSalaVisor) {
        codigoSalaVisor.innerText = "SALA " + salaAtual.toUpperCase();
    }

    const nomeSalvo = localStorage.getItem('jogador_nickname');
    if (nomeSalvo) {
        inputNome.value = nomeSalvo;
    }

    inputNome.addEventListener("keypress", (e) => {
        if (e.key === "Enter") btnEntrar.click();
    });

    btnEntrar.onclick = () => {
        const nomeDigitado = inputNome.value.trim();
        if (nomeDigitado === "") {
            mostrarModal("<h2 style='color: #ef4444;'>Atenção</h2><p>Você precisa digitar um nome.</p>", 2000);
            return;
        }

        meuNome = nomeDigitado;
        localStorage.setItem('jogador_nickname', meuNome);
        modalEntrada.classList.add("modal-oculto"); 

        const tituloJogador = document.querySelector('.info-local h2');
        if (tituloJogador) {
            tituloJogador.innerText = `${meuNome} (Você)`;
            let avatarLocal = document.querySelector('.local-avatar');
            if (!avatarLocal) {
                avatarLocal = document.createElement('img');
                avatarLocal.className = 'avatar-imagem local-avatar';
                tituloJogador.parentNode.insertBefore(avatarLocal, tituloJogador);
            }
            avatarLocal.src = `/assets/avatares/avatar_${configJogo.avatar}.png`;
            avatarLocal.alt = 'Seu Avatar';
        }
        
        iniciarConexaoFDP(); 
    };
});

// ==========================================
// CONEXÃO COM O SERVIDOR (WEBSOCKET)
// ==========================================
function iniciarConexaoFDP() {
    const protocolo = location.protocol === "https:" ? "wss" : "ws";
    socket = new WebSocket(`${protocolo}://${window.location.host}/ws/fdp`);

    socket.onopen = function() {
        console.log("WebSocket FDP conectado");
        
        let token = sessionStorage.getItem(`fdp_token_${salaAtual}`) || "NOVO_TOKEN";

        socket.send(JSON.stringify({
            tipo: "ENTRAR_SALA",
            sala: salaAtual,
            token: token,
            jogador_id: meuId,
            nome: meuNome 
        }));
    };

    socket.onmessage = function(event) {
        const msg = JSON.parse(event.data);

        if (msg.erro || msg.tipo === "ERRO") {
            mostrarModal(`<h2 style='color: #ef4444;'>Ops!</h2><p>${msg.erro || msg.mensagem}</p>`, 3500);
            if(msg.mensagem && msg.mensagem.toLowerCase().includes("token")) {
                sessionStorage.removeItem(`fdp_token_${salaAtual}`);
            }
            return;
        }

        if (msg.tipo === "ENTRADA_CONFIRMADA" || msg.tipo === "RECONEXAO_CONFIRMADA") {
            if (msg.token_reconexao) {
                sessionStorage.setItem(`fdp_token_${salaAtual}`, msg.token_reconexao);
            }
            // ---> CORREÇÃO CRÍTICA: ATUALIZA O ID PARA O QUE O SERVIDOR MANDOU <---
            if (Number.isInteger(msg.meu_id)) {
                meuId = msg.meu_id;
            } else if (Number.isInteger(msg.jogador_id)) {
                meuId = msg.jogador_id;
            }
            return; 
        }

        if(!msg.jogadores) return;
        atualizarInterface(msg);
    };

    socket.onclose = function() {
        console.warn("WebSocket fechado.");
        const painelConexao = document.getElementById('painel-conexao');
        if (painelConexao) {
            painelConexao.style.display = 'flex';
            painelConexao.classList.remove('conectado');
            painelConexao.classList.add('desconectado');
            document.querySelector('.indicador-conexao').style.backgroundColor = '#ef4444';
            document.getElementById('texto-conexao').innerText = "Conexão perdida";
        }
    };
}

// ==========================================
// ATUALIZAÇÃO DA INTERFACE DO JOGO
// ==========================================
function atualizarInterface(dados) {
    const estadoAnterior = window.estadoMesaAtual; 
    let iniciarAnimacao = false; 

    // ---> ATUALIZAÇÃO DO PAINEL DE CONEXÃO <---
    const painelConexao = document.getElementById('painel-conexao');
    if (painelConexao) {
        painelConexao.classList.remove('desconectado');
        painelConexao.classList.add('conectado');
        document.querySelector('.indicador-conexao').style.backgroundColor = '#4ade80';
        document.getElementById('texto-conexao').innerText = "Conectado";
        document.getElementById('texto-jogadores').innerText = `Jogadores: ${dados.jogadores.length}/${dados.max_jogadores || 4}`;
        
        if (dados.partida_iniciada) {
            painelConexao.style.display = 'none'; // Some com o painel quando o jogo começa
        } else {
            document.getElementById('texto-turno').innerText = "Aguardando iniciar...";
        }
    }

    const isRodadaCega = (dados.cartas_na_rodada === 1);
    const eu = dados.jogadores.find(j => j.id === meuId);
    if (!eu) return; // Agora isso não vai mais falhar!

    const qtdVivos = dados.jogadores.filter(j => j.vidas > 0).length;
    const euAnterior = estadoAnterior ? estadoAnterior.jogadores.find(j => j.id === meuId) : null;
    
    if (!euAnterior && eu.mao.length > 0) {
        iniciarAnimacao = true;
    } else if (euAnterior && eu.mao.length > euAnterior.mao.length) {
        iniciarAnimacao = true;
    }

    if (estadoAnterior && estadoAnterior.cartas_na_mesa && estadoAnterior.cartas_na_mesa.length > 0 && (!dados.cartas_na_mesa || dados.cartas_na_mesa.length === 0)) {
        const perdedores = dados.jogadores.filter(j => {
            const oldJ = estadoAnterior.jogadores.find(o => o.id === j.id);
            return oldJ && j.vidas < oldJ.vidas;
        });

        if (perdedores.length > 0) {
            let msg = "<h2 style='color: #ef4444;'>🩸 FIM DA RODADA</h2><hr style='border-color: #334155; margin-bottom: 15px;'>";
            perdedores.forEach(j => {
                const oldJ = estadoAnterior.jogadores.find(o => o.id === j.id);
                const perdeu = oldJ.vidas - j.vidas;
                msg += `<p><b>${j.name || j.nome || 'Jogador '+j.id}</b> perdeu ${perdeu} vida(s)</p>`;
            });
            const vivos = dados.jogadores.filter(j => j.vidas > 0);
            if (vivos.length === 1) {
                msg += `<h2 style='color: #4ade80; margin-top: 25px;'>👑 ${vivos[0].name || vivos[0].nome} VENCEU A PARTIDA! 👑</h2>`;
            }
            mostrarModal(msg, 2500); 
            tocarSom('knife-cut.mp3');
        } else {
            const ganhador = dados.jogadores.find(j => j.id === dados.jogador_da_vez_index);
            if (ganhador) {
                mostrarModal(`<h2 style='color: #60a5fa;'>✨ ${ganhador.name || ganhador.nome || 'Jogador '+ganhador.id} levou a vaza!</h2>`, 2500);
                tocarSom('victory_6.mp3');
            }
        }
    }

    window.estadoMesaAtual = dados; 

    document.getElementById('local-vidas').innerText = eu.vidas;
    document.getElementById('local-ganhas').innerText = eu.vezes_ganhas;
    const apostaLocalStr = eu.aposta_atual === -1 ? '<span class="pontinhos"></span>' : eu.aposta_atual;
    document.getElementById('local-aposta').innerHTML = apostaLocalStr;

    const ehMinhaVez = (dados.jogador_da_vez_index === meuId);
    const statusBox = document.getElementById('status-local');
    
    statusBox.classList.remove('sua-vez', 'morto');
    if (eu.vidas <= 0) {
        statusBox.classList.add('morto');
    } else if (ehMinhaVez) {
        statusBox.classList.add('sua-vez');
    }
    
    document.getElementById('cadeira-direita').innerHTML = '';
    document.getElementById('cadeira-topo').innerHTML = '';
    document.getElementById('cadeira-esquerda').innerHTML = '';

    const totalJogadores = dados.jogadores.length;

    dados.jogadores.forEach(j => {
        if (j.id !== meuId) {
            let posicaoRelativa = (j.id - meuId + totalJogadores) % totalJogadores;
            let idCadeira = "";

            if (totalJogadores === 4) {
                if (posicaoRelativa === 1) idCadeira = 'cadeira-direita';
                else if (posicaoRelativa === 2) idCadeira = 'cadeira-topo';
                else if (posicaoRelativa === 3) idCadeira = 'cadeira-esquerda';
            } else if (totalJogadores === 3) {
                if (posicaoRelativa === 1) idCadeira = 'cadeira-direita';
                else if (posicaoRelativa === 2) idCadeira = 'cadeira-esquerda';
            }

            if (idCadeira) {
                const cadeiraDiv = document.getElementById(idCadeira);
                let htmlCartasVerso = '<div class="mao-oponente">';
                const totalCartasOponente = j.mao.length;
                const meioDaMaoOponente = (totalCartasOponente - 1) / 2;

                for (let c = 0; c < totalCartasOponente; c++) {
                    const angulo = (c - meioDaMaoOponente) * 8; 
                    const transY = Math.abs(c - meioDaMaoOponente) * 3;
                    const styleStr = `style="--rotacao-oponente: ${angulo}deg; --transY-oponente: ${transY}px; z-index: ${c + 1};"`;

                    if (isRodadaCega && j.mao[c] && j.mao[c].valor) {
                        let cartaClasses = "carta mini-carta";
                        let attrNaipe = "";
                        let conteudo = "";

                        if (configJogo.baralho === 'padrao') {
                            cartaClasses += ` padrao-css ${j.mao[c].naipe}`;
                            attrNaipe = `data-naipe-simbolo="${obterSimboloNaipe(j.mao[c].naipe)}"`;
                            conteudo = `<span>${traduzirValorCarta(j.mao[c].valor)}</span> <div class="naipe-central">${obterSimboloNaipe(j.mao[c].naipe)}</div>`;
                        } else {
                            conteudo = `<img src="${obterCaminhoCarta(j.mao[c].valor, j.mao[c].naipe)}" class="carta-sprite">`;
                        }

                        htmlCartasVerso += `<div class="${cartaClasses}" ${attrNaipe} ${styleStr}>${conteudo}</div>`;
                    } else {
                        htmlCartasVerso += `<div class="carta-verso" ${styleStr}></div>`;
                    }
                }
                htmlCartasVerso += '</div>';

                const apostaOponenteStr = j.aposta_atual === -1 ? '<span class="pontinhos"></span>' : j.aposta_atual;
                const caminhoAvatar = obterCaminhoAvatar(j.id);
                
                const isOponenteMorto = (j.vidas <= 0);
                const isOponenteVez = (dados.jogador_da_vez_index === j.id && !isOponenteMorto);
                let classesPerfil = "perfil-jogador";
                if (isOponenteMorto) classesPerfil += " morto";
                else if (isOponenteVez) classesPerfil += " sua-vez";

                cadeiraDiv.innerHTML = `
                    <div class="${classesPerfil}">
                        ${isOponenteMorto ? '<div class="marcador-morte">❌</div>' : ''}
                        <img src="${caminhoAvatar}" class="avatar-imagem" alt="Avatar">
                        <h3>${j.name || j.nome || 'Jogador ' + j.id}</h3>
                        <div class="status">Vidas: ${j.vidas} | Aposta: ${apostaOponenteStr}</div>
                    </div>
                    ${htmlCartasVerso}
                `;
            }
        }
    });

    const viraDiv = document.getElementById('carta-vira');
    viraDiv.className = `carta`; 
    if (dados.carta_vira && dados.carta_vira.valor) {
        construirFaceCarta(viraDiv, dados.carta_vira);
        viraDiv.style.display = "flex";
    } else {
        viraDiv.innerHTML = "";
    }

    const mesaDiv = document.getElementById('cartas-na-mesa');
    if (mesaDiv) {
        mesaDiv.innerHTML = ''; 
        if (dados.cartas_na_mesa) {
            dados.cartas_na_mesa.forEach((carta, indice) => {
                const elementoCarta = document.createElement('div');
                elementoCarta.className = `carta`;
                
                construirFaceCarta(elementoCarta, carta);
                
                const rotacoes = [-4, 5, -2, 4];
                const transY = (indice % 2 === 0) ? -3 : 3;
                elementoCarta.style.setProperty('--rotacao-mesa', `${rotacoes[indice % 4]}deg`);
                elementoCarta.style.setProperty('--transY-mesa', `${transY}px`);
                elementoCarta.style.zIndex = indice + 1;
                
                mesaDiv.appendChild(elementoCarta);
            });
        }
    }

    const minhaMaoDiv = document.getElementById('minha-mao');
    minhaMaoDiv.innerHTML = '';
    const totalMinhasCartas = eu.mao.length;
    const meioDaMao = (totalMinhasCartas - 1) / 2;
    
    eu.mao.forEach((carta, indice) => {
        const elementoCarta = document.createElement('div');
        const angulo = (indice - meioDaMao) * 6;
        const translateY = Math.abs(indice - meioDaMao) * 4;
        
        elementoCarta.style.setProperty('--rotacao', `${angulo}deg`);
        elementoCarta.style.setProperty('--transY', `${translateY}px`);
        elementoCarta.style.zIndex = indice + 1;
        
        if (isRodadaCega) {
            elementoCarta.className = 'minha-carta-cega';
        } else {
            elementoCarta.className = `carta`;
            construirFaceCarta(elementoCarta, carta);
        }
        
        if (ehMinhaVez && dados.jogadores_que_ja_apostaram >= qtdVivos && eu.vidas > 0) {
            elementoCarta.onmousedown = () => jogarCarta(indice);
        } else {
            elementoCarta.style.opacity = "0.6";
            elementoCarta.style.cursor = "not-allowed";
        }
        minhaMaoDiv.appendChild(elementoCarta);
    });

    const painelApostas = document.getElementById('painel-apostas');
    if (ehMinhaVez && dados.jogadores_que_ja_apostaram < qtdVivos && eu.aposta_atual === -1 && eu.vidas > 0) {
        painelApostas.classList.remove('escondido');
        const containerBotoes = document.getElementById('botoes-aposta');
        containerBotoes.innerHTML = ''; 
        
        for (let i = 0; i <= dados.cartas_na_rodada; i++) {
            const btn = document.createElement('button');
            btn.className = 'btn-aposta';
            btn.innerText = i;
            if (dados.aposta_proibida === i) {
                btn.classList.add('proibido');
                btn.onmousedown = () => { tocarSom('click.mp3'); mostrarModal(`<h3 style='color:red;'>Bloqueado!</h3><p>O número ${i} causaria um empate na soma final.</p>`, 1500); };
            } else {
                btn.onmousedown = () => enviarAposta(i);
            }
            containerBotoes.appendChild(btn);
        }
    } else {
        painelApostas.classList.add('escondido');
    }

    const eraRodadaCega = (estadoAnterior && estadoAnterior.cartas_na_rodada === 1);
    if (isRodadaCega && !eraRodadaCega) {
        mostrarModal("<h2 style='color: #a855f7;'>🙈 Rodada Cega!</h2><p>Você não vê sua própria carta, mas vê as cartas na testa dos oponentes!</p>", 3000);
        tocarSom('funny_82hiegE.mp3');
    }

    if (iniciarAnimacao) animarDistribuicao(dados);
}

// ==========================================
// FUNÇÕES AUXILIARES E GERADORES DE CARTAS
// ==========================================

function construirFaceCarta(el, cartaBase) {
    if (configJogo.baralho === 'padrao') {
        const textoValor = traduzirValorCarta(cartaBase.valor);
        const simboloNaipe = obterSimboloNaipe(cartaBase.naipe);
        
        el.classList.add('padrao-css', cartaBase.naipe);
        el.setAttribute('data-naipe-simbolo', simboloNaipe);
        
        el.innerHTML = `<span>${textoValor}</span> <div class="naipe-central">${simboloNaipe}</div>`;
    } else {
        el.classList.remove('padrao-css', 'paus', 'copas', 'espadas', 'ouros');
        el.removeAttribute('data-naipe-simbolo');
        el.innerHTML = `<img src="${obterCaminhoCarta(cartaBase.valor, cartaBase.naipe)}" class="carta-sprite">`;
    }
}

function traduzirValorCarta(valor) {
    const mapa = { 1: "A", 11: "J", 12: "Q", 13: "K" };
    return mapa[valor] || valor;
}

function obterSimboloNaipe(naipeTexto) {
    const simbolos = { "paus": "♣", "copas": "♥", "espadas": "♠", "ouros": "♦" };
    return simbolos[naipeTexto] || "";
}

function obterCaminhoAvatar(id) {
    if (id === meuId) return `/assets/avatares/avatar_${configJogo.avatar}.png`;
    const numAvatar = (id % 4) + 1; 
    return `/assets/avatares/avatar_${numAvatar}.png`;
}

function obterCaminhoCarta(valor, naipe) {
    return `/assets/baralhos/${configJogo.baralho}/${valor}_${naipe}.png`;
}

// ==========================================
// FUNÇÕES DE AÇÕES DO JOGO
// ==========================================

function jogarCarta(indice) {
    tocarSom('jogar_carta.ogg');
    socket.send(JSON.stringify({ tipo: "ACAO_JOGO", acao: "JOGAR_CARTA", jogador_id: meuId, indice: indice })); 
}

function enviarAposta(valorDesejado) {
    tocarSom('click.mp3');
    socket.send(JSON.stringify({ tipo: "ACAO_JOGO", acao: "APOSTAR", jogador_id: meuId, valor: valorDesejado })); 
}

function mostrarModal(htmlContent, tempoMs) {
    const modal = document.getElementById('modal-notificacao');
    const texto = document.getElementById('modal-texto');
    texto.innerHTML = htmlContent;
    modal.classList.remove('modal-oculto');
    if (window.modalTimer) clearTimeout(window.modalTimer);
    window.modalTimer = setTimeout(() => { modal.classList.add('modal-oculto'); }, tempoMs);
}

// ==========================================
// CONTROLE DO MENU DE PERSONALIZAÇÃO
// ==========================================

function abrirConfig() {
    document.getElementById('modal-config').classList.remove('modal-oculto');
    
    for (let i = 1; i <= 4; i++) {
        const img = document.getElementById(`opt-avatar-${i}`);
        if(img) img.src = `/assets/avatares/avatar_${i}.png`;
    }

    document.querySelectorAll('.avatar-opcao').forEach(img => img.classList.remove('selecionado'));
    const btnAvatar = document.getElementById(`opt-avatar-${configJogo.avatar}`);
    if(btnAvatar) btnAvatar.classList.add('selecionado');
}

function fecharConfig() {
    document.getElementById('modal-config').classList.add('modal-oculto');
}

function aplicarConfiguracoes() {
    document.body.className = `tema-verso-${configJogo.verso}`;
}

function mudarVerso(tipo) { 
    configJogo.verso = tipo; 
    localStorage.setItem('pref_verso', tipo);
    aplicarConfiguracoes();
}

function mudarBaralho(tipo) { 
    configJogo.baralho = tipo; 
    localStorage.setItem('pref_baralho', tipo);
    if(window.estadoMesaAtual) atualizarInterface(window.estadoMesaAtual);
}

function mudarAvatar(num) {
    configJogo.avatar = num;
    localStorage.setItem('pref_avatar', num);
    
    document.querySelectorAll('.avatar-opcao').forEach(img => img.classList.remove('selecionado'));
    document.getElementById(`opt-avatar-${num}`).classList.add('selecionado');
    
    const localAvatarImg = document.querySelector('.local-avatar');
    if(localAvatarImg) localAvatarImg.src = `/assets/avatares/avatar_${num}.png`;
}

// ==========================================
// ANIMAÇÕES
// ==========================================

function animarDistribuicao(dados) {
    tocarSom('shuffle.mp3');
    const baralho = document.getElementById('baralho-central');
    if (!baralho) return;

    const rectBaralho = baralho.getBoundingClientRect();
    const origemX = rectBaralho.left + (rectBaralho.width / 2);
    const origemY = rectBaralho.top + (rectBaralho.height / 2);
    let delayDeLancamento = 0;

    dados.jogadores.forEach(j => {
        let idAlvo = "cadeira-base";
        if (j.id !== meuId) {
            const totalJogadores = dados.jogadores.length;
            let posicaoRelativa = (j.id - meuId + totalJogadores) % totalJogadores;
            if (totalJogadores === 4) {
                if (posicaoRelativa === 1) idAlvo = 'cadeira-direita';
                else if (posicaoRelativa === 2) idAlvo = 'cadeira-topo';
                else if (posicaoRelativa === 3) idAlvo = 'cadeira-esquerda';
            } else if (totalJogadores === 3) {
                if (posicaoRelativa === 1) idAlvo = 'cadeira-direita';
                else if (posicaoRelativa === 2) idAlvo = 'cadeira-esquerda';
            }
        }

        const assento = document.getElementById(idAlvo);
        if (!assento) return;
        const rectAssento = assento.getBoundingClientRect();
        const destinoX = rectAssento.left + (rectAssento.width / 2);
        const destinoY = rectAssento.top + (rectAssento.height / 2);

        for (let c = 0; c < j.mao.length; c++) {
            setTimeout(() => {
                const cartaVoadora = document.createElement('div');
                cartaVoadora.className = 'carta-verso carta-animada';
                cartaVoadora.style.left = origemX + 'px';
                cartaVoadora.style.top = origemY + 'px';
                cartaVoadora.style.transform = 'translate(-50%, -50%) scale(1)';
                document.body.appendChild(cartaVoadora);

                requestAnimationFrame(() => {
                    cartaVoadora.style.left = destinoX + 'px';
                    cartaVoadora.style.top = destinoY + 'px';
                    cartaVoadora.style.transform = 'translate(-50%, -50%) scale(0.5) rotate(720deg)';
                    cartaVoadora.style.opacity = '0';
                });

                setTimeout(() => cartaVoadora.remove(), 600);
            }, delayDeLancamento);
            delayDeLancamento += 80; 
        }
    });
}
