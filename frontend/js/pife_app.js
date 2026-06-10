const sons={jogar:new Audio("/assets/sons/jogar_carta.ogg"),shuffle:new Audio("/assets/sons/shuffle.mp3"),vitoria:new Audio("/assets/sons/victory_6.mp3"),click:new Audio("/assets/sons/click.mp3"),selecionar:new Audio("/assets/sons/selecionar.mp3")};

function tocarSom(nome){
    if(!sons[nome]) return;
    sons[nome].currentTime=0;
    sons[nome].play().catch(()=>{});
}

let socket=null,meuId=null,nomeJogador="",cartaArrastada=null,ordemLocalMao=[],cartaSelecionadaIndice=null,estadoAtualGlobal=null,jogoJaAcabou=false,tentativasReconexao=0,temporizadorReconexao=null,fechamentoManual=false;

const minhaMao=document.getElementById("minha-mao");
const monteCompra=document.getElementById("monte-compra");
const mesaDescarte=document.getElementById("mesa-descarte");
const cartaVira=document.getElementById("carta-vira");
const textoCoringa=document.getElementById("carta-coringa");
const qtdCartas=document.getElementById("qtd-cartas");
const jogadorAtualTexto=document.getElementById("jogador-atual");
const mensagemAjuda=document.getElementById("mensagem-ajuda");
const btnBater=document.getElementById("btn-bater");
const btnOrganizar=document.getElementById("btn-organizar");
const nomeJogadorLocal=document.getElementById("nome-jogador-local");
const painelConexao=document.getElementById("painel-conexao");
const pontoConexao=document.getElementById("ponto-conexao");
const textoConexao=document.getElementById("texto-conexao");
const jogadoresConectadosTexto=document.getElementById("jogadores-conectados");
const maxJogadoresTexto=document.getElementById("max-jogadores");
const textoEspera=document.getElementById("texto-espera");
const codigoSalaTexto=document.getElementById("codigo-sala");

const parametros=new URLSearchParams(location.search);
const sala=(parametros.get("sala")||"global").trim().toUpperCase();
const quantidadeJogadores=Number(parametros.get("jogadores")||2);
const chaveToken=`pife_token_${sala}`;
const chaveNome=`pife_nome_${sala}`;

let token=localStorage.getItem(chaveToken);
nomeJogador=localStorage.getItem(chaveNome)||"";

if(!token){
    token=typeof crypto.randomUUID==="function"?crypto.randomUUID():`${Date.now()}-${Math.random().toString(36).slice(2)}`;
    localStorage.setItem(chaveToken,token);
}

codigoSalaTexto.textContent=`Sala: ${sala}`;
maxJogadoresTexto.textContent=quantidadeJogadores;

if(nomeJogador){
    nomeJogadorLocal.textContent=`${nomeJogador} (Você)`;
    iniciarWebSocket();
}else{
    mostrarTelaNome();
}

function mostrarTelaNome(){
    if(document.getElementById("tela-nome")) return;

    const tela=document.createElement("div");
    tela.id="tela-nome";
    tela.innerHTML=`
        <div class="caixa-nome">
            <div class="naipes-nome">♠ ♥ ♦ ♣</div>
            <h1>PIFE</h1>
            <p>Digite seu nome para entrar</p>
            <input id="input-nome-jogador" type="text" maxlength="20" placeholder="Seu nome" autocomplete="off">
            <span id="erro-nome"></span>
            <button id="btn-confirmar-nome" type="button">Entrar</button>
        </div>
    `;

    tela.style.cssText="position:fixed;inset:0;z-index:2000;display:flex;align-items:center;justify-content:center;background:rgba(3,7,18,.92);backdrop-filter:blur(8px)";
    document.body.appendChild(tela);

    const caixa=tela.querySelector(".caixa-nome");
    const naipes=tela.querySelector(".naipes-nome");
    const titulo=tela.querySelector("h1");
    const texto=tela.querySelector("p");
    const input=tela.querySelector("#input-nome-jogador");
    const erro=tela.querySelector("#erro-nome");
    const botao=tela.querySelector("#btn-confirmar-nome");

    caixa.style.cssText="width:min(400px,90vw);padding:42px 38px;border:1px solid rgba(255,255,255,.14);border-radius:22px;background:linear-gradient(145deg,#162033,#0b1220);box-shadow:0 24px 70px rgba(0,0,0,.7);text-align:center";
    naipes.style.cssText="margin-bottom:10px;color:#f0c040;font-size:25px;letter-spacing:9px";
    titulo.style.cssText="margin-bottom:10px;color:#f8fafc;font-size:32px;letter-spacing:6px";
    texto.style.cssText="margin-bottom:22px;color:#aab4c3;font-size:14px";
    input.style.cssText="width:100%;padding:14px;border:1px solid #334155;border-radius:10px;outline:none;background:#0f172a;color:#fff;font-size:16px;text-align:center";
    erro.style.cssText="display:block;min-height:20px;margin:8px 0;color:#fca5a5;font-size:13px";
    botao.style.cssText="width:100%;padding:14px;border:1px solid #22c55e55;border-radius:10px;background:#166534;color:#fff;font-weight:700;letter-spacing:2px;cursor:pointer";

    const confirmar=()=>{
        const nome=input.value.trim().replace(/\s+/g," ").slice(0,20);

        if(!nome){
            erro.textContent="Digite um nome válido.";
            input.style.borderColor="#ef4444";
            input.focus();
            return;
        }

        nomeJogador=nome;
        localStorage.setItem(chaveNome,nomeJogador);
        nomeJogadorLocal.textContent=`${nomeJogador} (Você)`;
        tela.remove();
        iniciarWebSocket();
    };

    botao.onclick=confirmar;
    input.addEventListener("keydown",evento=>{
        if(evento.key==="Enter") confirmar();
    });

    input.addEventListener("input",()=>{
        erro.textContent="";
        input.style.borderColor="#334155";
    });

    setTimeout(()=>input.focus(),50);
}

function iniciarWebSocket(){
    if(!nomeJogador) return;
    if(socket&&(socket.readyState===WebSocket.OPEN||socket.readyState===WebSocket.CONNECTING)) return;

    fechamentoManual=false;
    atualizarConexao("Conectando...","conectando");
    textoEspera.textContent="Aguardando conexão";

    const protocolo=location.protocol==="https:"?"wss:":"ws:";
    socket=new WebSocket(`${protocolo}//${location.host}/ws/pife`);

    socket.onopen=()=>{
        tentativasReconexao=0;
        atualizarConexao("Conectado","conectado");
        textoEspera.textContent="Entrando na sala";

        socket.send(JSON.stringify({
            tipo:"entrar_sala",
            sala,
            token,
            nome:nomeJogador
        }));
    };

    socket.onmessage=evento=>{
        let msg;

        try{
            msg=JSON.parse(evento.data);
        }catch{
            console.error("Mensagem inválida recebida:",evento.data);
            return;
        }

        processarMensagem(msg);
    };

    socket.onerror=erro=>console.error("Erro no WebSocket:",erro);

    socket.onclose=()=>{
        atualizarConexao("Reconectando...","reconectando");
        textoEspera.textContent="Conexão perdida";

        if(fechamentoManual) return;

        tentativasReconexao++;
        clearTimeout(temporizadorReconexao);

        temporizadorReconexao=setTimeout(
            iniciarWebSocket,
            Math.min(tentativasReconexao*1000,5000)
        );
    };
}

function processarMensagem(msg){
    if(msg.tipo==="conectado") return;

    if(msg.tipo==="pong") return;

    if(msg.erro||msg.tipo==="erro"){
        mostrarModal(msg.erro||msg.mensagem||"Erro desconhecido");
        return;
    }

    if(msg.tipo==="entrada_confirmada"||msg.tipo==="reconexao_confirmada"){
        if(Number.isInteger(msg.idJogador)) meuId=msg.idJogador;

        if(msg.nome){
            nomeJogador=msg.nome;
            localStorage.setItem(chaveNome,nomeJogador);
            nomeJogadorLocal.textContent=`${nomeJogador} (Você)`;
        }

        atualizarConexao(
            msg.tipo==="reconexao_confirmada"?"Reconectado":"Conectado",
            "conectado"
        );

        jogadoresConectadosTexto.textContent=
            msg.jogadoresConectados ??
            msg.jogadores_conectados ??
            jogadoresConectadosTexto.textContent;

        maxJogadoresTexto.textContent=
            msg.maxJogadores ??
            msg.max_jogadores ??
            quantidadeJogadores;

        textoEspera.textContent=
            (msg.partidaIniciada??msg.partida_iniciada)
                ?"Partida iniciada"
                :"Aguardando jogadores";

        socket.send(JSON.stringify({tipo:"obter_estado"}));
        return;
    }

    if(Array.isArray(msg.minha_mao)){
        if(Number.isInteger(msg.meu_id)) meuId=msg.meu_id;
        renderizarEstado(msg);
    }
}

function atualizarConexao(texto,classe){
    textoConexao.textContent=texto;
    pontoConexao.className=`ponto-conexao ${classe}`;
}

function atualizarPainelConexao(estado){
    const jogadores=Array.isArray(estado.jogadores)?estado.jogadores:[];
    const conectados=estado.jogadores_conectados??jogadores.filter(j=>j.conectado!==false).length;
    const maximo=estado.max_jogadores??estado.jogadores_registrados??jogadores.length??quantidadeJogadores;

    jogadoresConectadosTexto.textContent=conectados;
    maxJogadoresTexto.textContent=maximo||quantidadeJogadores;

    if(estado.partida_iniciada){
        atualizarConexao("Partida iniciada","conectado");
        textoEspera.textContent="Todos os jogadores conectados";

        setTimeout(()=>{
            painelConexao.classList.add("oculto");
        },1200);
    }else{
        painelConexao.classList.remove("oculto");
        atualizarConexao("Conectado","conectado");
        textoEspera.textContent="Aguardando jogadores";
    }
}

function enviarAcao(acao,dados={}){
    if(!socket||socket.readyState!==WebSocket.OPEN){
        mostrarModal("Sem conexão com o servidor");
        return;
    }

    socket.send(JSON.stringify({
        tipo:"acao_jogo",
        acao,
        ...dados
    }));
}

function renderizarEstado(estado){
    estadoAtualGlobal=estado;

    atualizarPainelConexao(estado);
    renderizarMao(estado.minha_mao||[]);
    renderizarOponentes(estado);
    renderizarMesa(estado.mesa||[]);
    renderizarVira(estado.vira);

    qtdCartas.textContent=(estado.minha_mao||[]).length;

    if(estado.meu_nome){
        nomeJogador=estado.meu_nome;
        nomeJogadorLocal.textContent=`${nomeJogador} (Você)`;
        localStorage.setItem(chaveNome,nomeJogador);
    }

    if(!estado.partida_iniciada){
        jogadorAtualTexto.textContent="Aguardando jogadores";
        mensagemAjuda.textContent=`Aguardando jogadores: ${jogadoresConectadosTexto.textContent}/${maxJogadoresTexto.textContent}`;
    }else{
        jogadorAtualTexto.textContent=Number.isInteger(estado.jogador_atual)
            ?estado.jogador_atual===meuId?"Sua vez":obterNomeJogador(estado,estado.jogador_atual)
            :"Aguardando...";

        mensagemAjuda.textContent=estado.jogador_atual===meuId
            ?"Sua vez. Compre uma carta e depois descarte."
            :"Aguarde sua vez.";
    }

    atualizarBotoesEAreas(estado);
    verificarVitoria(estado);
    cartaSelecionadaIndice=null;
}

function obterNomeJogador(estado,id){
    const jogador=Array.isArray(estado.jogadores)
        ?estado.jogadores.find(j=>j.id===id)
        :null;

    return jogador?.nome||`Jogador ${id+1}`;
}

function renderizarOponentes(estado){
    ["cadeira-direita","cadeira-topo","cadeira-esquerda"].forEach(id=>{
        const cadeira=document.getElementById(id);
        if(cadeira) cadeira.innerHTML="";
    });

    const jogadores=Array.isArray(estado.jogadores)?estado.jogadores:[];
    const total=jogadores.length;

    jogadores.forEach(jogador=>{
        if(jogador.id===meuId) return;

        const posicao=(jogador.id-meuId+total)%total;
        let idCadeira="";

        if(total===4){
            if(posicao===1) idCadeira="cadeira-direita";
            else if(posicao===2) idCadeira="cadeira-topo";
            else if(posicao===3) idCadeira="cadeira-esquerda";
        }else if(total===3){
            if(posicao===1) idCadeira="cadeira-direita";
            else if(posicao===2) idCadeira="cadeira-esquerda";
        }else if(total===2){
            idCadeira="cadeira-topo";
        }

        if(!idCadeira) return;

        const cadeira=document.getElementById(idCadeira);
        const quantidade=jogador.quantidade_cartas??jogador.qtd_cartas??jogador.cartas_na_mao??0;
        const conectado=jogador.conectado!==false;
        const turno=estado.jogador_atual===jogador.id;
        const nome=jogador.nome||`Jogador ${jogador.id+1}`;

        let cartas='<div class="mao-oponente">';

        for(let i=0;i<quantidade;i++){
            cartas+='<div class="carta-verso"></div>';
        }

        cartas+="</div>";

        cadeira.innerHTML=`
            <h2 class="${turno?"jogador-em-turno":""} ${conectado?"":"jogador-desconectado"}">
                ${nome}${conectado?"":" — desconectado"}
            </h2>
            ${cartas}
        `;
    });
}

function renderizarMao(cartas){
    minhaMao.innerHTML="";

    const lista=cartas.map((carta,indice)=>({
        ...carta,
        idx:indice,
        id:`${carta.valor}-${carta.naipe}-${indice}`
    }));

    lista.sort((a,b)=>{
        const posicaoA=ordemLocalMao.indexOf(a.id);
        const posicaoB=ordemLocalMao.indexOf(b.id);

        return (posicaoA===-1?999:posicaoA)-(posicaoB===-1?999:posicaoB);
    });

    lista.forEach(item=>{
        const elemento=criarCarta(item);

        elemento.dataset.idx=item.idx;
        elemento.dataset.id=item.id;
        elemento.draggable=true;

        elemento.ondragstart=()=>{
            cartaArrastada=elemento;
            cartaSelecionadaIndice=item.idx;
            elemento.classList.add("dragging");
            tocarSom("selecionar");
        };

        elemento.ondragend=()=>{
            elemento.classList.remove("dragging");
            cartaArrastada=null;

            ordemLocalMao=[...minhaMao.querySelectorAll(".carta")]
                .map(carta=>carta.dataset.id);
        };

        elemento.onclick=()=>{
            minhaMao.querySelectorAll(".carta")
                .forEach(carta=>carta.classList.remove("selecionada"));

            elemento.classList.add("selecionada");
            cartaSelecionadaIndice=item.idx;
            tocarSom("selecionar");
        };

        minhaMao.appendChild(elemento);
    });
}

minhaMao.ondragover=evento=>{
    evento.preventDefault();

    if(!cartaArrastada) return;

    const cartas=[...minhaMao.querySelectorAll(".carta:not(.dragging)")];
    const posterior=cartas.find(carta=>{
        const retangulo=carta.getBoundingClientRect();
        return evento.clientX<=retangulo.left+retangulo.width/2;
    });

    posterior
        ?minhaMao.insertBefore(cartaArrastada,posterior)
        :minhaMao.appendChild(cartaArrastada);
};

function renderizarMesa(cartas){
    mesaDescarte.innerHTML="";

    if(!cartas.length){
        mesaDescarte.innerHTML='<div class="carta carta-vazia">Descarte</div>';
        return;
    }

    mesaDescarte.appendChild(criarCarta(cartas[cartas.length-1]));
}

function renderizarVira(vira){
    cartaVira.innerHTML="";

    if(!vira||!vira.valor){
        cartaVira.innerHTML='<div class="carta carta-vazia">Vira</div>';
        textoCoringa.textContent="aguardando";
        return;
    }

    cartaVira.appendChild(criarCarta(vira));

    const valor=Number(vira.valor);

    textoCoringa.textContent=cartaParaTexto({
        valor:valor===13?1:valor+1,
        naipe:vira.naipe
    });
}

function criarCarta(carta){
    const imagem=document.createElement("img");

    imagem.className="carta";
    imagem.src=caminhoImagemCarta(carta);
    imagem.alt=cartaParaTexto(carta);
    imagem.draggable=false;

    imagem.onerror=()=>{
        console.error("Imagem não encontrada:",imagem.src);
    };

    return imagem;
}

function caminhoImagemCarta(carta){
    const naipes={0:"clubs",1:"hearts",2:"spades",3:"diamonds"};
    const valores={1:"ace",2:"02",3:"03",4:"04",5:"05",6:"06",7:"07",8:"08",9:"09",10:"10",11:"jack",12:"queen",13:"king"};

    return `/assets/cartas/${naipes[Number(carta.naipe)]}_${valores[Number(carta.valor)]}.png`;
}

function cartaParaTexto(carta){
    const valores={1:"A",11:"J",12:"Q",13:"K"};
    const naipes={0:"♣",1:"♥",2:"♠",3:"♦"};

    return `${valores[carta.valor]||carta.valor}${naipes[carta.naipe]||""}`;
}

function atualizarBotoesEAreas(estado){
    const jogando=Boolean(estado.partida_iniciada)&&!estado.jogo_finalizado;

    btnBater.disabled=!jogando||!estado.pode_bater;
    btnOrganizar.disabled=!jogando;

    document.querySelector(".status-box")?.classList.toggle(
        "sua-vez",
        jogando&&estado.jogador_atual===meuId
    );

    monteCompra.classList.toggle(
        "ativo",
        jogando&&Boolean(estado.pode_comprar_baralho)
    );

    mesaDescarte.classList.toggle(
        "ativo",
        jogando&&(
            Boolean(estado.pode_comprar_mesa)||
            Boolean(estado.pode_descartar)
        )
    );
}

monteCompra.onclick=()=>{
    if(!estadoAtualGlobal?.pode_comprar_baralho) return;

    tocarSom("selecionar");
    enviarAcao("COMPRAR_BARALHO");
};

mesaDescarte.onclick=()=>{
    if(estadoAtualGlobal?.pode_comprar_mesa){
        tocarSom("selecionar");
        enviarAcao("COMPRAR_MESA");
        return;
    }

    if(cartaSelecionadaIndice!==null&&estadoAtualGlobal?.pode_descartar){
        tocarSom("jogar");
        enviarAcao("DESCARTAR",{indice:cartaSelecionadaIndice});
        cartaSelecionadaIndice=null;
    }
};

mesaDescarte.addEventListener("dragover",evento=>{
    evento.preventDefault();

    if(cartaSelecionadaIndice!==null&&estadoAtualGlobal?.pode_descartar){
        mesaDescarte.classList.add("drag-over");
    }
});

mesaDescarte.addEventListener("dragleave",()=>{
    mesaDescarte.classList.remove("drag-over");
});

mesaDescarte.addEventListener("drop",evento=>{
    evento.preventDefault();
    mesaDescarte.classList.remove("drag-over");

    if(cartaSelecionadaIndice===null||!estadoAtualGlobal?.pode_descartar) return;

    tocarSom("jogar");
    enviarAcao("DESCARTAR",{indice:cartaSelecionadaIndice});
    cartaSelecionadaIndice=null;
});

btnBater.onclick=()=>{
    if(!estadoAtualGlobal?.pode_bater) return;

    tocarSom("click");
    enviarAcao("BATER");
};

btnOrganizar.onclick=()=>{
    if(!estadoAtualGlobal?.partida_iniciada||estadoAtualGlobal?.jogo_finalizado) return;

    tocarSom("click");
    ordemLocalMao=[];
    enviarAcao("ORGANIZAR");
};

function verificarVitoria(estado){
    if(estado.jogo_finalizado&&!jogoJaAcabou){
        jogoJaAcabou=true;

        if(estado.vencedor===meuId){
            mostrarModal("Você bateu! 🎉");
            tocarSom("vitoria");
        }else if(Number.isInteger(estado.vencedor)){
            mostrarModal(`${obterNomeJogador(estado,estado.vencedor)} bateu!`);
        }else{
            mostrarModal("Partida finalizada");
        }
    }

    if(!estado.jogo_finalizado) jogoJaAcabou=false;
}

function mostrarModal(texto){
    const modal=document.getElementById("modal-notificacao");
    const modalTexto=document.getElementById("modal-texto");

    if(!modal||!modalTexto){
        console.log(texto);
        return;
    }

    modalTexto.textContent=texto;
    modal.classList.remove("modal-oculto");

    clearTimeout(mostrarModal.timeout);

    mostrarModal.timeout=setTimeout(()=>{
        modal.classList.add("modal-oculto");
    },2200);
}