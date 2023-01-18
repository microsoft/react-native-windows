
window.addEventListener('load', () => {
    let navToggle = document.querySelector("#navButtons_clickSource");
    let docsNavContainer = document.querySelector("#navButtons_clickTarget");
    if (navToggle && docsNavContainer) {
        navToggle.addEventListener("click", (e)=>{
                docsNavContainer.classList.toggle("docsSliderActive");
            });
    }
});