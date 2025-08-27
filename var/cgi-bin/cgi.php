#!/usr/bin/php-cgi
<?php
// header('Content-Typey: text/html; charset=utf-8');
// echo "\r\n\r\n";
$method = $_SERVER['REQUEST_METHOD'] ?? 'UNKNOWN';
$query = $_SERVER['QUERY_STRING'] ?? '';
$length = $_SERVER['CONTENT_LENGTH'] ?? 0;
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
		
		<?php if ($method === 'POST'): ?>
			<h2>Données POST reçues :</h2>
			<pre>
			<?php 
				if (!empty($_POST)) {
					echo htmlspecialchars($_POST['name']);
				} else {
					echo "Aucune donnée POST reçue";
				}
			?>
			</pre>
		<?php endif; ?>

		<?php if ($method === 'GET'): ?>
			<h2>Données GET reçues :</h2>
			<pre>
			<?php 
				if (!empty($_GET)) {
					echo htmlspecialchars($_GET['name']);
				} else {
					echo "Aucune donnée GET reçue";
				}
			?>
			</pre>
		<?php endif; ?>

		<hr>
		<h3>Variables d'environnement :</h3>
		<div class="env-vars">
			<?php
				echo "Method: " . htmlspecialchars($method) . "\n";
				echo "Query String: " . htmlspecialchars($query) . "\n";
				echo "Content Length: " . htmlspecialchars($length) . "\n";
			?>
		</div>

		<a href="/cgi.html" class="btn"><button type="button">Retour</button></a>
	</div>
</body>
</html>