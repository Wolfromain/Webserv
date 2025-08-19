#!/usr/bin/php-cgi
<?php
header('Content-Type: text/html; charset=utf-8');
?>
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Test CGI PHP</title>
    <link rel="stylesheet" href="/styles/style.css">
</head>
<body>
    <div class="container">
        <h1>Test CGI PHP</h1>
        
        <?php if ($_SERVER['REQUEST_METHOD'] === 'POST'): ?>
            <h2>Données POST reçues :</h2>
            <pre>
            <?php print_r($_POST); ?>
            </pre>
        <?php endif; ?>

        <?php if ($_SERVER['REQUEST_METHOD'] === 'GET'): ?>
            <h2>Données GET reçues :</h2>
            <pre>
            <?php print_r($_GET); ?>
            </pre>
        <?php endif; ?>

        <hr>
        <h3>Variables d'environnement :</h3>
        <pre>
        <?php
            echo "Method: " . $_SERVER['REQUEST_METHOD'] . "\n";
            echo "Query String: " . $_SERVER['QUERY_STRING'] . "\n";
            echo "Content Length: " . $_SERVER['CONTENT_LENGTH'] . "\n";
        ?>
        </pre>
    </div>
</body>
</html>