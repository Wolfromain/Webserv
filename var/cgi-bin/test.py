import os
import cgi
import html
import shutil
import sys

UPLOAD_DIR = "var/www/styles/uploads"

print("Content-Type: text/html; charset=utf-8\r\n\r\n")

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
<h1>Resultat de l'envoi</h1>
""")


name = form.getvalue("name", "").strip()

print("<html><body>")

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
	print(f"<p>Image recue : <strong>{filename}</strong></p>")
	print(f"<p>Image affichee ci-dessous :</p>")
	print(f'<img src="{public_url}" alt="Image envoyée" width="200">')

print("</body></html>")
