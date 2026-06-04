// Function to update network configuration based on the data received from the server
function updateNetworkConfig(data) {
    document.getElementById('physical-address').textContent = data.physical_address;
    document.getElementById('ipv4-address').textContent = data.ipv4_address;
    document.getElementById('subnet-mask').textContent = data.subnet_mask;
    document.getElementById('default-gateway').textContent = data.default_gateway;
}

// Function to fetch network configuration from the server
function fetchNetworkConfig() {
    fetch('/network')
        .then(response => response.json())
        .then(data => {
            updateNetworkConfig(data);
        })
        .catch(error => {
            console.error('Error fetching network configuration:', error);
        });
}

// Function to update LED status based on the data received from the server
function updateLedStatus(data) {
    // Update LED1 status and indicator
    const led1Indicator = document.getElementById('led1-indicator');
    const led1Status = document.getElementById('led1-status');
    if (data.led1) {
        led1Indicator.classList.remove('btn-secondary');
        led1Indicator.classList.add('btn-danger');
        led1Status.textContent = 'LED1 ON';
    } else {
        led1Indicator.classList.remove('btn-danger');
        led1Indicator.classList.add('btn-secondary');
        led1Status.textContent = 'LED1 OFF';
    }

    // Update LED2 status and indicator
    const led2Indicator = document.getElementById('led2-indicator');
    const led2Status = document.getElementById('led2-status');
    if (data.led2) {
        led2Indicator.classList.remove('btn-secondary');
        led2Indicator.classList.add('btn-danger');
        led2Status.textContent = 'LED2 ON';
    } else {
        led2Indicator.classList.remove('btn-danger');
        led2Indicator.classList.add('btn-secondary');
        led2Status.textContent = 'LED2 OFF';
    }
}

// Function to fetch LED status from the server
function fetchLedStatus() {
    fetch('/led')
        .then(response => response.json())
        .then(data => {
            updateLedStatus(data);
        })
        .catch(error => {
            console.error('Error fetching LED status:', error);
        });
}

// Function to push the switch click event to the server
function pushEvent(url) {
    fetch(url)
        .then(response => {
            fetchLedStatus();
        })
        .catch(error => {
            console.error('Error push switch click on:', error);
        });
}

// Function to fetch network configuration and LED status from the server on page load
function fetchPage(){
    fetchNetworkConfig();
    fetchLedStatus();
    setInterval(() => {fetchLedStatus();}, 1000);
}