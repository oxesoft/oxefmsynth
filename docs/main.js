/**
 * Oxe FM Synth - Official Website Interactivity
 */

// 1. Copy Code Helper with micro-interaction blur crossfade
function copyCode(btn, text) {
  if (!navigator.clipboard) return;

  navigator.clipboard.writeText(text).then(() => {
    const label = btn.querySelector(".copy-btn-text") || btn;
    const originalText = label.textContent;

    // Blur masks the text change during state transition
    label.classList.add("transitioning");
    setTimeout(() => {
      label.textContent = "Copied!";
      btn.classList.add("copied");
      label.classList.remove("transitioning");
    }, 90);

    setTimeout(() => {
      label.classList.add("transitioning");
      setTimeout(() => {
        label.textContent = originalText;
        btn.classList.remove("copied");
        label.classList.remove("transitioning");
      }, 90);
    }, 2000);
  });
}

// 2. Lightbox Modal for Screenshot
document.addEventListener("DOMContentLoaded", () => {
  const modal = document.getElementById("imgModal");
  const modalImg = document.getElementById("modalImg");
  const screenshotBtn = document.getElementById("screenshotBtn");
  const screenshotImg = document.getElementById("mainScreenshot");
  const modalClose = document.getElementById("modalClose");

  const openModal = () => {
    if (screenshotImg && modalImg) {
      modalImg.src = screenshotImg.src;
    }
    modal.classList.add("active");
    if (modalClose) {
      modalClose.focus();
    }
  };

  const closeModal = () => {
    modal.classList.remove("active");
    if (screenshotBtn) {
      screenshotBtn.focus();
    }
  };

  if (screenshotBtn && modal) {
    screenshotBtn.addEventListener("click", openModal);
  } else if (screenshotImg && modal) {
    screenshotImg.addEventListener("click", openModal);
  }

  if (modalClose) {
    modalClose.addEventListener("click", closeModal);
  }

  if (modal) {
    modal.addEventListener("click", (e) => {
      if (e.target === modal) {
        closeModal();
      }
    });

    document.addEventListener("keydown", (e) => {
      if (e.key === "Escape" && modal.classList.contains("active")) {
        closeModal();
      }
    });
  }
});
