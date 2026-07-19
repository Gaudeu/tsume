import tkinter as tk
import os

from tkinter import *
from tkinter import ttk, scrolledtext, messagebox

import socket

import subprocess
import threading

server_process = None


#funcoes
def get_ip():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80)) 
        ip_local = s.getsockname()[0]
        s.close()
        return ip_local
    except Exception:
        return "localHost addr"
    
    
def edit_port():
    janela_pop = tk.Toplevel(root)
    janela_pop.title("config Port")
    janela_pop.grab_set()

    frm_pop = ttk.Frame(janela_pop, padding=15)
    frm_pop.grid(row=0, column=0, sticky="nsew")

    tk.Label(frm_pop, text="be careful! a port should be a number between 1024 and 65535").grid(column=0, row=0,columnspan=2, pady=(0,10), sticky="w")
    tk.Label(frm_pop, text="New Port:").grid(column=0, row=1, pady=5, sticky="w")

    entry_port = ttk.Entry(frm_pop, width=10)
    entry_port.insert(0, str(REAL_PORT))
    entry_port.grid(column=1, row=1, pady=5, padx=5, sticky="w")
    entry_port.focus()

    def save_port():
        global REAL_PORT
        entry_text = entry_port.get()

        if entry_text.isdigit(): 
            REAL_PORT = int(entry_text)
            if dados_visiveis:
                var_port.set(str(REAL_PORT))
            janela_pop.destroy()
        else:
            
            entry_port.delete(0, tk.END)

    btn_ok = ttk.Button(frm_pop, text="OK", command=save_port)
    btn_ok.grid(column=0, row=2, padx=2, pady=10, sticky="we")

    btn_cancel = ttk.Button(frm_pop, text="Cancel", command=janela_pop.destroy)
    btn_cancel.grid(column=1, row=2, padx=2, pady=10, sticky="we")

    janela_pop.update_idletasks()  
    janela_pop.resizable(False, False)


def toggle_view():
    global dados_visiveis
    if dados_visiveis:
        # Se estava visível, esconde
        var_ip.set("•••••••••••••")
        var_port.set("••••")
        btn_show_param.config(image=img_visible)
        dados_visiveis = False
    else:
        var_ip.set(IP_REAL)
        var_port.set(str(REAL_PORT))
        btn_show_param.config(image=img_NONvisible)
        dados_visiveis = True

def read_out():
    global server_process

    def screen_update(texto):
        log_box.insert(tk.END, texto)
        log_box.see(tk.END)

    while server_process and server_process.poll() is None:
        linha = server_process.stdout.readline()
        if not linha:
            break

        root.after(0, screen_update, linha)
        if "[NETWORK] request received" in linha:
            # Pede para a interface abrir a caixa de diálogo
            root.after(0, conexao_request)

def conexao_request():
    resposta = messagebox.askyesno("New connection", "a 3ds is trying to connect, accept?")

    if server_process and server_process.poll() is None:
        comand = "y\n" if resposta else "n\n"
        server_process.stdin.write(comand)
        server_process.stdin.flush()

        if resposta:
            
            var_status.set("server active")
        else:
            
            var_status.set("awaiting 3ds...")


def start_server():
    global serverON, server_process

    if serverON:
       var_status.set("server inactive")
       serverON = False
       var_btnServer.set("open server")

       if server_process:
           server_process.terminate()
           server_process = None
           log_box.insert(tk.END, "\n--- PROCESS ENDED ---\n")
        
       btn_alt.grid()
       rb_esquerdo.state(['!disabled']) 
       rb_direito.state(['!disabled'])


    else:
        var_status.set("awaiting 3DS...")
        serverON = True
        var_btnServer.set("end server")

        btn_alt.grid_remove()
        rb_esquerdo.state(['disabled'])
        rb_direito.state(['disabled'])

        log_box.delete(1.0, tk.END)

        gui_folder = os.path.dirname(os.path.abspath(__file__))

        exe_path = os.path.abspath(os.path.join(gui_folder, "..", "server"))

        server_process = subprocess.Popen(
            [exe_path, str(REAL_PORT), var_stick.get()],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,bufsize=1
        )

        thread_reader = threading.Thread(target=read_out, daemon=True)
        thread_reader.start()

def close_app():

    if serverON:

        response = messagebox.askyesno("Exit", "server is active. are you sure?")
        if response:

            if server_process:
                server_process.terminate()
            root.destroy()
    else:
        root.destroy()
    



root = tk.Tk()
root.title("server")
root.geometry("390x410")

gui_folder = os.path.dirname(os.path.abspath(__file__))

img_visible = tk.PhotoImage(file=os.path.join(gui_folder, "visible.png"))
img_NONvisible = tk.PhotoImage(file=os.path.join(gui_folder, "NONvisible.png"))


IP_REAL = get_ip()
REAL_PORT = 1234
dados_visiveis = False
serverON = False

var_ip = tk.StringVar(value="•••••••••••••")
var_port = tk.StringVar(value="••••")
var_status = tk.StringVar(value="server offline")
var_btnServer =  tk.StringVar(value="activate server")
var_stick = tk.StringVar(value="0")

frm = ttk.Frame(root, padding = 15)
frm.grid(row=0, column=0, sticky="nsew")

ttk.Label(frm, text="Thumbstick:").grid(column=0, row=3, pady=5, sticky="w")
frm_radio = ttk.Frame(frm)
frm_radio.grid(column=1, row=3, columnspan=2, sticky="w")

rb_esquerdo = ttk.Radiobutton(frm_radio, text="Left", variable=var_stick, value="0")
rb_esquerdo.grid(column=0, row=0, padx=(0, 10))

rb_direito = ttk.Radiobutton(frm_radio, text="Right", variable=var_stick, value="1")
rb_direito.grid(column=1, row=0)

# columnspan=2 faz o título ocupar o topo inteiro
ttk.Label(frm, text="SERVER STATUS", font=("Arial", 10, "bold")).grid(column=0, row=0, columnspan=2, pady=(0, 15), sticky="w")


ttk.Label(frm, text="IP:").grid(column=0, row=1, pady=5, sticky="w")
ttk.Label(frm, textvariable=var_ip, foreground="blue").grid(column=1, row=1, pady=5, sticky="w")

ttk.Label(frm, text="Port:").grid(column=0, row=2, pady=5, sticky="w")
ttk.Label(frm, textvariable=var_port).grid(column=1, row=2, pady=5, sticky="w")
#ttk.Button(frm, text="alterar porta", command=root.destroy).grid(column=3, row=2)

ttk.Label(frm, text="Status:").grid(column=0, row=4, pady=5, sticky="w")
ttk.Label(frm, textvariable=var_status, font=("Arial", 9, "italic")).grid(column=1, row=4, pady=5, sticky="w")



btn_show_param = ttk.Button(frm, image=img_visible, command=toggle_view, width=4)
btn_show_param.grid(column=2, row=1, padx=5)
btn_show_param.image_on = img_visible
btn_show_param.image_off = img_NONvisible
btn_alt = ttk.Button(frm, text="Change Port", command=edit_port)
btn_alt.grid(column=2, row=2, pady=(15, 5), padx=2, sticky="we")

log_box = scrolledtext.ScrolledText(frm, height=8, width=50, bg="black", fg="white", font=("Consolas", 9))
log_box.grid(column=0, row=5, columnspan=3, pady=(15, 10), sticky="we")

btn_help = ttk.Button(frm, text="Help", command=close_app)
btn_help.grid(column=0, row=6, pady=(15, 5), padx=2, sticky="we")

btn_quit = ttk.Button(frm, text="Quit", command=close_app)
btn_quit.grid(column=2, row=6, pady=(15, 5), padx=2, sticky="we")

btn_start = ttk.Button(frm, textvariable=var_btnServer, command=start_server)
btn_start.grid(column=1, row=6, pady=(15, 5), padx=2, sticky="we")

root.protocol("WM_DELETE_WINDOW", close_app)
root.mainloop()
