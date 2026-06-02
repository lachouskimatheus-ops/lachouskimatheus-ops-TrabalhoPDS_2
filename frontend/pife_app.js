let cartaSelecionada = null;

const cartas = document.querySelectorAll("#minha-mao .carta");
const mesaDescarte = document.getElementById("mesa-descarte");
const qtdCartas = document.getElementById("qtd-cartas");

cartas.forEach((carta) => {
    carta.addEventListener("click", () => {
        if (cartaSelecionada) {
            cartaSelecionada.classList.remove("selecionada");
        }

        cartaSelecionada = carta;
        cartaSelecionada.classList.add("selecionada");
    });
});

document.getElementById("btn-comprar-monte").addEventListener("click", () => {
    mostrarModal("Você comprou uma carta do monte.");
});

document.getElementById("btn-comprar-mesa").addEventListener("click", () => {
    mostrarModal("Você comprou a carta da mesa.");
});

document.getElementById("btn-bater").addEventListener("click", () => {
    mostrarModal("Você tentou bater.");
});

document.getElementById("btn-organizar").addEventListener("click", () => {
    mostrarModal("Mão organizada.");
});

mesaDescarte.addEventListener("click", () => {
    if (!cartaSelecionada) {
        mostrarModal("Selecione uma carta antes de descartar.");
        return;
    }

    mesaDescarte.innerHTML = "";
    mesaDescarte.appendChild(cartaSelecionada);

    cartaSelecionada.classList.remove("selecionada");
    cartaSelecionada = null;

    atualizarQuantidadeCartas();
});

function atualizarQuantidadeCartas() {
    const total = document.querySelectorAll("#minha-mao .carta").length;
    qtdCartas.textContent = total;
}

function mostrarModal(texto) {
    const modal = document.getElementById("modal-notificacao");
    const modalTexto = document.getElementById("modal-texto");

    modalTexto.textContent = texto;
    modal.classList.remove("modal-oculto");

    setTimeout(() => {
        modal.classList.add("modal-oculto");
    }, 1200);
}