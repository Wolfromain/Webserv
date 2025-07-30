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

delete_filename = form.getvalue("delete")
if delete_filename:
	delete_path = os.path.join(UPLOAD_DIR, os.path.basename(delete_filename))
	if os.path.exists(delete_path):
		os.remove(delete_path)
		print(f"<p style='color: green;'>✅ Image supprimée : {html.escape(delete_filename)}</p>")
	else:
		print(f"<p style='color: red;'>❌ Fichier introuvable : {html.escape(delete_filename)}</p>")
	print('<a href="/" class="back-button">⬅️ Retour</a>')
	print("</div></body></html>")
	sys.exit(0)

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
	print(f''' <form method="POST" action="/cgi-bin/test.py">
		<input type="hidden" name="delete" value="{html.escape(filename)}">
		<button type="submit" class="delete-button">🗑️ Supprimer cette image</button>
		</form> ''')

print('<br><a href="/" class="back-button">⬅️ Retour</a>')
print("</body></html>")
