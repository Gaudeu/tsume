import tkinter as tk
from tkinter import ttk

# 1. Configuração da Janela Principal
root = tk.Tk()
root.title("server")
root.geometry("300x250")  # Aumentei um pouco para caber os botões confortavelmente

# 2. Criando o Container Principal
frm = ttk.Frame(root, padding=20)
frm.grid(row=0, column=0, sticky="nsew") # Posiciona o frame na célula principal

# 3. Rótulos de Texto (Labels) - Organizados por Linha (row) e Coluna (column)
ttk.Label(frm, text="SERVER CONTROL", font=("Arial", 12, "bold")).grid(column=0, row=0, columnspan=2, pady=10)

ttk.Label(frm, text="IP:").grid(column=0, row=1, sticky="w")
ttk.Label(frm, text="127.0.0.1").grid(column=1, row=1, sticky="w") # Exemplo de valor

ttk.Label(frm, text="Port:").grid(column=0, row=2, sticky="w")


import_port_entry = ttk.Entry(frm, width=10)
import_port_entry.insert(0, "8080") # Porta padrão preenchida
import_port_entry.grid(column=1, row=2, sticky="w")

button_start = ttk.Button(frm, text="Start Server")
button_start.grid(column=0, row=3, pady=10)

button_change = ttk.Button(frm, text="Alterar Porta")
button_change.grid(column=1, row=3, pady=10)

button_quit = ttk.Button(frm, text="Quit", command=root.destroy)
button_quit.grid(column=0, row=4, columnspan=2, sticky="we", pady=5)


root.mainloop()