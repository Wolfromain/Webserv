import os
import cgi
import html
import shutil
import sys

UPLOAD_DIR = "var/www/styles/uploads"

print("Content-Type: text/html; charset=utf-8\r\n\r\n")

form = cgi.FieldStorage()
name = form.getvalue("name", "").strip()

print("<html><body>")
print("<h1>Résultat de l'envoi</h1>")

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

	public_url = f"/uploads/{filename}"

	print(f"<p>Pseudo Minecraft : <strong>{safe_name}</strong></p>")
	print(f"<p>Image reçue : <strong>{filename}</strong></p>")
	print(f"<p>Image affichée ci-dessous :</p>")
	print(f'<img src="{public_url}" alt="Image envoyée" width="200">')

print("</body></html>")
