const std::string my_html_page = R"(<!DOCTYPE html>
<html>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>Digital Boring Head - DRO</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 400px;
            margin: 20px auto;
            padding: 20px;
            background-color: #f0f0f0;
        }
        .dro-display {
            background-color: #000;
            color: #0f0;
            font-family: 'Courier New', monospace;
            font-size: 48px;
            font-weight: bold;
            text-align: right;
            padding: 15px;
            border: 3px solid #333;
            border-radius: 5px;
            margin: 10px 0;
            letter-spacing: 6px;
        }
        .label {
            font-size: 14px;
            font-weight: bold;
            margin-top: 15px;
            margin-bottom: 5px;
        }
        .control-buttons {
            display: flex;
            flex-direction: row-reverse;
            align-items: left;
            margin: 15px 0;
        }
        .btn {
            font-size: 24px;
            font-family: 'Courier New', monospace;
            padding: 10px 20px;
            margin: 5px;
            border: 2px solid #333;
            border-radius: 5px;
            background-color: #ddd;
            cursor: pointer;
            min-width: 60px;
        }
        .btn:active {
            background-color: #bbb;
        }
        .goto-btn {
            width: 100%;
            font-size: 20px;
            padding: 20px;
            margin-top: 20px;
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
        }
        .goto-btn:active {
            background-color: #45a049;
        }
    </style>
</head>
<body>
    <div class='dro-display' id='currentPos'>0.00</div>
    <div class='dro-display' id='targetPos'>0.00</div>
    <div class='control-buttons'; margin-bottom: 4px>
        <div style='display: flex; gap: 5px; margin-bottom: 1px;'>
            <button class='btn' onclick='adjustTarget(10)'>+</button>
            <button class='btn' onclick='adjustTarget(1)'>+</button>
            <button class='btn' onclick='adjustTarget(0.1)'>+</button>
            <button class='btn' onclick='adjustTarget(0.01)'>+</button>
        </div>
    </div>
    <div class='control-buttons'; margin-top: 4px>
        <div style='display: flex; gap: 5px;'>
            <button class='btn' onclick='adjustTarget(-10)'>-</button>
            <button class='btn' onclick='adjustTarget(-1)'>-</button>
            <button class='btn' onclick='adjustTarget(-0.1)'>-</button>
            <button class='btn' onclick='adjustTarget(-0.01)'>-</button>
        </div>
    </div>
    
    <button class='goto-btn' onclick='goToPosition()'>GO TO</button>

    <button class='goto-btn' onclick='setCurrentPosition()' style='background-color: #2196F3; margin-top: 10px;'>SET CURRENT POSITION</button>
    
    <script>
        let targetValue = 0.00;
        
        function adjustTarget(delta) {
            targetValue += delta;
            targetValue = Math.round(targetValue * 100) / 100;
            document.getElementById('targetPos').textContent = targetValue.toFixed(2);
        }
        
        function goToPosition() {
            const url = '/goto?pos=' + targetValue.toFixed(2);
            fetch(url, { method: 'PUT' })
                .then(response => {
                    if (response.ok) {
                        updatePosition();
                    } else {
                        alert('Error sending goto command');
                    }
                })
                .catch(error => {
                    alert('Network error: ' + error);
                });
        }

        function setCurrentPosition() {
            const url = '/pos?pos=' + targetValue.toFixed(2);
            fetch(url, { method: 'PUT' })
                .then(response => {
                    if (response.ok) {
                        updatePosition();
                    } else {
                        alert('Error setting current position');
                    }
                })
                .catch(error => {
                    alert('Network error: ' + error);
                });
        }
        
        function updatePosition() {
            fetch('/pos')
                .then(response => response.json())
                .then(data => {
                    const currentMm = data.pos_current_mm.toFixed(2);
                    document.getElementById('currentPos').textContent = currentMm;
                })
                .catch(error => {
                    console.error('Error fetching position:', error);
                });
        }
        
        // Update position every 500ms
        setInterval(updatePosition, 2000);
        // Initial position update
        updatePosition();
    </script>
</body>
</html>)";
