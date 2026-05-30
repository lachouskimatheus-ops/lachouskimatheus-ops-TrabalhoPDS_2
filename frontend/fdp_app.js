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
    if (eu.mao.length > 0 && (!euAnterior || euAnterior.mao.length === 0)) {
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
                msg += `<p><b>${j.name || j.nome || 'Jogador '+j.id}</b> perdeu ${perdeu} vida(s)</p>`;
            });
            
            const vivos = dados.jogadores.filter(j => j.vidas > 0);
            if (vivos.length === 1) {
                msg += `<h2 style='color: #4ade80; margin-top: 25px;'>👑 ${vivos[0].name || vivos[0].nome} VENCEU A PARTIDA! 👑</h2>`;
            }
            
            mostrarModal(msg, 5000); 
        } else {
            const ganhador = dados.jogadores.find(j => j.id === dados.jogador_da_vez_index);
            if (ganhador) {
                mostrarModal(`<h2 style='color: #60a5fa;'>✨ ${ganhador.name || ganhador.nome || 'Jogador '+ganhador.id} levou a vaza!</h2>`, 2500);
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
            elementoCarta.onclick = () => jogarCarta(indice);
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
                btn.onclick = () => alert(`O número ${i} está bloqueado!`);
            } else {
                btn.onclick = () => enviarAposta(i);
            }
            
            containerBotoes.appendChild(btn);
        }
    } else {
        painelApostas.classList.add('escondido');
    }

    // Dispara o efeito visual e o alerta de rodada cega
    if (iniciarAnimacao) {
        animarDistribuicao(dados);
        
        if (isRodadaCega) {
            mostrarModal("<h2 style='color: #a855f7;'>🙈 Rodada Cega!</h2><p>Você não vê sua própria carta, mas vê as cartas na testa dos oponentes!</p>", 6000);
        }
    }
}

// ==========================================
// FUNÇÕES AUXILIARES E ANIMAÇÃO
// ==========================================
function jogarCarta(indice) {
    socket.send(JSON.stringify({
        acao: "JOGAR_CARTA",
        jogador_id: meuId,
        indice: indice
    }));
}

function enviarAposta(valorDesejado) {
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