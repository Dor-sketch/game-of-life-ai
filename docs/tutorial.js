document.getElementById('help-btn').addEventListener('click', function () {
    const tutorialPopup = document.getElementById('tutorial-popup');
    tutorialPopup.style.display = 'block'; // Use block if flex doesn't work as expected
    tutorialPopup.style.zIndex = '1000'; // Ensure it's above other content

    showStep(1);
});

document.querySelectorAll('.next-btn').forEach(btn => {
    btn.addEventListener('click', function () {
        const currentStep = parseInt(this.closest('.tutorial-step').getAttribute('data-step'));
        console.log('Current step:', currentStep);
        showStep(currentStep + 1);
    });
});

document.querySelectorAll('.close-btn').forEach(btn => {
    btn.addEventListener('click', function () {
        closeTutorialPopup();
    });
});

document.getElementById('tutorial-popup').addEventListener('click', function (e) {
    if (e.target === this) {
        closeTutorialPopup();
    }
});

function showStep(step) {
    document.querySelectorAll('.tutorial-step').forEach(stepEl => {
        stepEl.style.display = 'none';
    });
    const nextStepElement = document.querySelector(`.tutorial-step[data-step="${step}"]`);
    if (nextStepElement) {
        nextStepElement.style.display = 'block';
    }
}

function closeTutorialPopup() {
    document.getElementById('tutorial-popup').style.display = 'none';
}