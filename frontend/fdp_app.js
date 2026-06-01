// ==========================================
// SISTEMA DE ÁUDIO DE ALTA PERFORMANCE
// ==========================================

// 1. Cria um "banco" na memória RAM para guardar os sons
const bancoDeSons = {};

// 2. Função para carregar o som na RAM antes do jogo começar
function preCarregarSom(nomeDoArquivo) {
    bancoDeSons[nomeDoArquivo] = new Audio(`./assets/sons/${nomeDoArquivo}`);
    bancoDeSons[nomeDoArquivo].preload = 'auto'; // Força o Firefox a decodificar agora
}

// 3. Carrega os sons que precisam ser rápidos como um raio (adicione outros se quiser)
preCarregarSom('click.ogg');
preCarregarSom('jogar_carta.ogg');

// 4. A nova função tocarSom
function tocarSom(nomeDoArquivo) {
    let audio;
    
    if (bancoDeSons[nomeDoArquivo]) {
        // Se já está na RAM, clona instantaneamente! 
        // (O clone permite tocar o mesmo som várias vezes sobrepostas sem engasgar)
        audio = bancoDeSons[nomeDoArquivo].cloneNode();
    } else {
        // Sons menos importantes que não foram pré-carregados seguem o fluxo normal
        audio = new Audio(`./assets/sons/${nomeDoArquivo}`);
    }
    
    audio.volume = 0.5; 
    
    audio.play().catch(erro => {
        console.log("O navegador bloqueou o som automático. O usuário precisa interagir com a tela antes.");
    });
};

// 1. Resgata o nome da memória do navegador (ou define "Anônimo" se vier vazio)
const meuNome = localStorage.getItem('jogador_nickname') || 'Anônimo';

// 2. Atualiza a sua própria tela localmente
document.addEventListener("DOMContentLoaded", () => {
    const tituloJogador = document.querySelector('.info-local h2');
    if (tituloJogador) {
        tituloJogador.innerText = `${meuNome} (Você)`;
    }
});

// 1. Pega o ID do jogador
const urlParams = new URLSearchParams(window.location.search);
const meuId = parseInt(urlParams.get('id')) || 0;

// 2. Conecta ao servidor C++
const socket = new WebSocket('ws://localhost:8080/ws');

socket.onmessage = function(event) {
    const estadoMesa = JSON.parse(event.data);
    atualizarInterface(estadoMesa);
};

function atualizarInterface(dados) {
    const estadoAnterior = window.estadoMesaAtual; 
    let iniciarAnimacao = false; 

    // DETECTOR DE RODADA CEGA (1 Carta)
    const isRodadaCega = (dados.cartas_na_rodada === 1);

    const eu = dados.jogadores.find(j => j.id === meuId);
    if (!eu) return;

    // DETETIVE DE DISTRIBUIÇÃO
    const euAnterior = estadoAnterior ? estadoAnterior.jogadores.find(j => j.id === meuId) : null;
    
    if (!euAnterior && eu.mao.length > 0) {
        // Primeira rodada de todas (quando entra na mesa)
        iniciarAnimacao = true;
    } else if (euAnterior && eu.mao.length > euAnterior.mao.length) {
        // Nova rodada! Se o número de cartas AUMENTOU em relação à jogada anterior,
        // é porque o carteador do C++ acabou de dar as cartas novas!
        iniciarAnimacao = true;
    }

    // ==========================================
    // DETETIVE DE VAZAS E RODADAS
    // ==========================================
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
                msg += `<p><b>${j.name || j.nome || 'Jogador '+j.id}</b> perdeu ${perdeu} vida</p>`;
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
    document.getElementById('local-aposta').innerText = eu.aposta_atual === -1 ? '?' : eu.aposta_atual;
    document.getElementById('local-ganhas').innerText = eu.vezes_ganhas;

    const ehMinhaVez = (dados.jogador_da_vez_index === meuId);
    
    // ==========================================
    // ATUALIZA OPONENTES NAS CADEIRAS
    // ==========================================
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
                for (let c = 0; c < j.mao.length; c++) {
                    // LÓGICA DA RODADA CEGA AQUI
                    if (isRodadaCega && j.mao[c] && j.mao[c].valor) {
                        const textoValor = traduzirValorCarta(j.mao[c].valor);
                        const simboloNaipe = obterSimboloNaipe(j.mao[c].naipe);
                        htmlCartasVerso += `<div class="carta mini-carta ${j.mao[c].naipe}" data-naipe-simbolo="${simboloNaipe}">${textoValor}</div>`;
                    } else {
                        htmlCartasVerso += '<div class="carta-verso"></div>';
                    }
                }
                htmlCartasVerso += '</div>';

                cadeiraDiv.innerHTML = `
                    <div class="perfil-jogador ${dados.jogador_da_vez_index === j.id ? 'sua-vez' : ''}">
                        <h3>${j.name || j.nome || 'Jogador ' + j.id}</h3>
                        <div class="status">Vidas: ${j.vidas} | Apostas: ${j.aposta_atual === -1 ? '?' : j.aposta_atual}</div>
                    </div>
                    ${htmlCartasVerso}
                `;
            }
        }
    });

    // ==========================================
    // 1. ATUALIZA A CARTA VIRA
    // ==========================================
    const viraDiv = document.getElementById('carta-vira');
    if (dados.carta_vira && dados.carta_vira.valor) {
        const textoValor = traduzirValorCarta(dados.carta_vira.valor);
        const simboloNaipe = obterSimboloNaipe(dados.carta_vira.naipe);
        
        viraDiv.innerText = `${textoValor}`;
        viraDiv.className = `carta ${dados.carta_vira.naipe}`;
        viraDiv.setAttribute('data-naipe-simbolo', simboloNaipe);
        viraDiv.style.display = "flex";
    } else {
        viraDiv.innerText = "?";
        viraDiv.className = "carta";
        viraDiv.removeAttribute('data-naipe-simbolo');
    }

    // ==========================================
    // 2. ATUALIZA AS CARTAS JOGADAS NA MESA
    // ==========================================
    const mesaDiv = document.getElementById('cartas-na-mesa');
    if (mesaDiv) {
        mesaDiv.innerHTML = ''; 
        if (dados.cartas_na_mesa) {
            dados.cartas_na_mesa.forEach(carta => {
                const elementoCarta = document.createElement('div');
                const textoValor = traduzirValorCarta(carta.valor);
                const simboloNaipe = obterSimboloNaipe(carta.naipe);
                
                elementoCarta.className = `carta ${carta.naipe}`;
                elementoCarta.innerText = `${textoValor}`;
                elementoCarta.setAttribute('data-naipe-simbolo', simboloNaipe);
                
                mesaDiv.appendChild(elementoCarta);
            });
        }
    }

    // ==========================================
    // 3. RENDERIZA A SUA MÃO DE CARTAS
    // ==========================================
    const minhaMaoDiv = document.getElementById('minha-mao');
    minhaMaoDiv.innerHTML = '';
    
    eu.mao.forEach((carta, indice) => {
        const elementoCarta = document.createElement('div');
        
        // LÓGICA DA SUA CARTA NA RODADA CEGA
        if (isRodadaCega) {
            elementoCarta.className = 'minha-carta-cega';
        } else {
            const textoValor = traduzirValorCarta(carta.valor);
            const simboloNaipe = obterSimboloNaipe(carta.naipe);

            elementoCarta.className = `carta ${carta.naipe}`;
            elementoCarta.innerText = `${textoValor}`;
            elementoCarta.setAttribute('data-naipe-simbolo', simboloNaipe);
        }
        
        if (ehMinhaVez && dados.jogadores_que_ja_apostaram >= dados.jogadores.length) {
            elementoCarta.onmousedown = () => jogarCarta(indice);
        } else {
            elementoCarta.style.opacity = "0.6";
            elementoCarta.style.cursor = "not-allowed";
        }
        
        minhaMaoDiv.appendChild(elementoCarta);
    });

    // ==========================================
    // 4. CONTROLA O PAINEL DE APOSTAS
    // ==========================================
    const painelApostas = document.getElementById('painel-apostas');
    if (ehMinhaVez && dados.jogadores_que_ja_apostaram < dados.jogadores.length && eu.aposta_atual === -1) {
        painelApostas.classList.remove('escondido');
        
        const containerBotoes = document.getElementById('botoes-aposta');
        containerBotoes.innerHTML = ''; 
        
        for (let i = 0; i <= dados.cartas_na_rodada; i++) {
            const btn = document.createElement('button');
            btn.className = 'btn-aposta';
            btn.innerText = i;
            
            if (dados.aposta_proibida === i) {
                btn.classList.add('proibido');
                btn.onmousedown = () => {
                    tocarSom('click.ogg');
                    alert(`O número ${i} está bloqueado!`);
                };
            } else {
                btn.onmousedown = () => enviarAposta(i);
            }
            
            containerBotoes.appendChild(btn);
        }
    } else {
        painelApostas.classList.add('escondido');
    }

    // ==========================================
    // ALERTA DA RODADA CEGA E SOM NO INÍCIO
    // ==========================================
    const eraRodadaCega = (estadoAnterior && estadoAnterior.cartas_na_rodada === 1);

    if (isRodadaCega && !eraRodadaCega) {
        mostrarModal("<h2 style='color: #a855f7;'>🙈 Rodada Cega!</h2><p>Você não vê sua própria carta, mas vê as cartas na testa dos oponentes!</p>", 3000);
        tocarSom('funny_82hiegE.mp3');
    }

    // Dispara o efeito visual das cartas voando
    if (iniciarAnimacao) {
        animarDistribuicao(dados);
    }
}

// ==========================================
// FUNÇÕES AUXILIARES E ANIMAÇÃO
// ==========================================
function jogarCarta(indice) {
    tocarSom('jogar_carta.ogg');
    socket.send(JSON.stringify({
        acao: "JOGAR_CARTA",
        jogador_id: meuId,
        indice: indice
    }));
}

function enviarAposta(valorDesejado) {
    tocarSom('click.mp3');
    socket.send(JSON.stringify({
        acao: "APOSTAR",
        jogador_id: meuId,
        valor: valorDesejado
    }));
}

function traduzirValorCarta(valor) {
    const mapa = { 1: "A", 11: "J", 12: "Q", 13: "K" };
    return mapa[valor] || valor;
}

function obterSimboloNaipe(naipeTexto) {
    const simbolos = { "paus": "♣", "copas": "♥", "espadas": "♠", "ouros": "♦" };
    return simbolos[naipeTexto] || "";
}

function mostrarModal(htmlContent, tempoMs) {
    const modal = document.getElementById('modal-notificacao');
    const texto = document.getElementById('modal-texto');
    
    texto.innerHTML = htmlContent;
    modal.classList.remove('modal-oculto');
    
    if (window.modalTimer) clearTimeout(window.modalTimer);
    
    window.modalTimer = setTimeout(() => {
        modal.classList.add('modal-oculto');
    }, tempoMs);
}

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