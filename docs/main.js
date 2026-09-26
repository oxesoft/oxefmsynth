/**
 * Oxe FM Synth - Official Website Interactivity
 */

// 1. Copy Code Helper
function copyCode(btn, text) {
  navigator.clipboard.writeText(text).then(() => {
    const originalText = btn.textContent;
    btn.textContent = "Copied!";
    btn.style.color = "var(--accent-green)";
    setTimeout(() => {
      btn.textContent = originalText;
      btn.style.color = "";
    }, 2000);
  });
}

// 2. Lightbox Modal for Screenshot
document.addEventListener("DOMContentLoaded", () => {
  const modal = document.getElementById("imgModal");
  const modalImg = document.getElementById("modalImg");
  const screenshot = document.getElementById("mainScreenshot");
  const modalClose = document.getElementById("modalClose");

  if (screenshot && modal && modalImg) {
    screenshot.addEventListener("click", () => {
      modal.classList.add("active");
      modalImg.src = screenshot.src;
    });

    if (modalClose) {
      modalClose.addEventListener("click", () => {
        modal.classList.remove("active");
      });
    }

    modal.addEventListener("click", (e) => {
      if (e.target === modal) {
        modal.classList.remove("active");
      }
    });

    document.addEventListener("keydown", (e) => {
      if (e.key === "Escape" && modal.classList.contains("active")) {
        modal.classList.remove("active");
      }
    });
  }
});
