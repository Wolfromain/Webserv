#!/usr/bin/env python3
import os
import cgi
import html
import shutil
import sys
import urllib.parse


UPLOAD_DIR = "var/www/styles/uploads"

request_method = os.environ.get("REQUEST_METHOD", "GET")

print("\r\n\r\n")

if request_method == "DELETE":
	query = os.environ.get("QUERY_STRING", "")
	params = urllib.parse.parse_qs(query)
	delete_filename = params.get("file", [None])[0]
		
	print("<html><head><meta charset='UTF-8'><title>Suppression</title></head>")
	print("<body><div class='container'>")
		
	if delete_filename:
		delete_path = os.path.join(UPLOAD_DIR, os.path.basename(delete_filename))
		if os.path.exists(delete_path):
			os.remove(delete_path)
			print(f"<p style='color: green;'>✅ Image supprimée : {html.escape(delete_filename)}</p>")
			
			if os.path.isdir(UPLOAD_DIR) and not os.listdir(UPLOAD_DIR):
				os.rmdir(UPLOAD_DIR)
				print("<p style='color: orange;'>📁 Dossier uploads supprimé car vide.</p>")
		else:
			print(f"<p style='color: red;'>❌ Fichier introuvable : {html.escape(delete_filename)}</p>")
	else:
		print("<p style='color: red;'>❌ Aucun fichier spécifié.</p>")
		
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
		
	os.makedirs(UPLOAD_DIR, exist_ok=True)
	save_path = os.path.join(UPLOAD_DIR, filename)
		
	with open(save_path, 'wb') as f:
		shutil.copyfileobj(fileitem.file, f)
		
	public_url = f"/styles/uploads/{filename}"
		
	print(f"<p>Pseudo Minecraft : <strong>{safe_name}</strong></p>")
	print(f"<p>Image reçue : <strong>{filename}</strong></p>")
	print(f'<img src="{public_url}" alt="Image envoyée" width="200">')

	print(f'''
	<button onclick="deleteImage('{html.escape(filename)}')" class="delete-button">
		🗑️ Supprimer cette image
	</button>
	<script>
	function deleteImage(filename) {{
		if (confirm('Êtes-vous sûr de vouloir supprimer cette image ?')) {{
			fetch('/cgi-bin/test.py?file=' + encodeURIComponent(filename), {{
				method: 'DELETE'
			}})
			.then(response => response.text())
			.then(data => {{
				document.body.innerHTML = data;
				setTimeout(() => {{
					window.location.href = '/upload.html';
				}}, 2000);
			}})
			.catch(error => {{
				console.error('Erreur:', error);
				alert('Erreur lors de la suppression');
			}});
		}}
	}}
	</script>
	''')
print('<br><a href="/upload.html" class="back-button">⬅️ Retour à l\'upload</a>')
print("</div></body></html>")
