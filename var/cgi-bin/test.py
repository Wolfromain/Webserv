#!/usr/bin/env python3
import os
import cgi
import html
import shutil
import sys
import urllib.parse

UPLOAD_DIR = "var/www/styles/uploads"

print("Content-Type: text/html; charset=utf-8\r\n\r\n")

method = os.environ.get("REQUEST_METHOD", "GET")

if method == "DELETE":
	query = os.environ.get("QUERY_STRING", "")
	params = urllib.parse.parse_qs(query)
	delete_filename = params.get("file", [None])[0]

	print("<html><body><div class='container'>")
	if delete_filename:
		delete_path = os.path.join(UPLOAD_DIR, os.path.basename(delete_filename))
		if os.path.exists(delete_path):
			os.remove(delete_path)
			print(f"<p style='color: green;'>✅ Image supprimée : {html.escape(delete_filename)}</p>")
		else:
			print(f"<p style='color: red;'>❌ Fichier introuvable : {html.escape(delete_filename)}</p>")
	else:
		print("<p style='color: red;'>❌ Aucun fichier spécifié.</p>")
	print('<a href="/" class="back-button">⬅️ Retour</a>')
	print("</div></body></html>")
	sys.exit(0)

form = cgi.FieldStorage()

print("""
<html>
	<head>
		<meta charset="UTF-8">
		<title>Résultat de l'envoi</title>
		<link href="https://fonts.cdnfonts.com/css/minecraft-4" rel="stylesheet">
		<link rel="stylesheet" href="/styles/python.css">
	</head>
	<body>
	<div class="container">
	<h1>Résultat de l'envoi</h1>
""")

name = form.getvalue("name", "").strip()

if not name:
	print("<p style='color: red;'>Erreur : Pseudo manquant.</p>")
elif "image" not in form or not form["image"].filename:
	print("<p style='color: red;'>Erreur : Aucune image sélectionnée.</p>")
else:
	fileitem = form["image"]
	filename = os.path.basename(fileitem.filename)
	safe_name = html.escape(name)

	save_path = os.path.join(UPLOAD_DIR, filename)
	with open(save_path, 'wb') as f:
		shutil.copyfileobj(fileitem.file, f)

	public_url = f"/styles/uploads/{filename}"

	print(f"<p>Pseudo Minecraft : <strong>{safe_name}</strong></p>")
	print(f"<p>Image reçue : <strong>{filename}</strong></p>")
	print(f'<img src="{public_url}" alt="Image envoyée" width="200">')

	print(f''' <button onclick="fetch('/cgi-bin/test.py?file={html.escape(filename)}', {{ method: 'DELETE' }})
		.then(() => window.location.href='/');" class="delete-button">
			🗑️ Supprimer cette image
		</button> ''')

print('<br><a href="/" class="back-button">⬅️ Retour</a>')
print("</div></body></html>")
