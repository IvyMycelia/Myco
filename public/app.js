function showMessage() {
    const messageDiv = document.getElementById('message');
    messageDiv.textContent = 'Hello from Myco Web Server! Static file serving is working perfectly!';
    messageDiv.style.display = 'block';
    
    // Add some animation
    messageDiv.style.opacity = '0';
    messageDiv.style.transition = 'opacity 0.5s';
    
    setTimeout(() => {
        messageDiv.style.opacity = '1';
    }, 100);
}

// Add some interactive features
document.addEventListener('DOMContentLoaded', function() {
    console.log('Myco Web Server - Static files loaded successfully!');
    
    // Add a click counter
    let clickCount = 0;
    const button = document.querySelector('button');
    
    button.addEventListener('click', function() {
        clickCount++;
        if (clickCount > 1) {
            const messageDiv = document.getElementById('message');
            messageDiv.textContent += ` (Clicked ${clickCount} times)`;
        }
    });
});
