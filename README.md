# -*- coding: utf-8 -*-
import tkinter as tk
from tkinter import filedialog, messagebox, ttk, scrolledtext
import psutil
import sys
import os
import shutil
import locale

class WTTaskCacheGUI:
    def __init__(self, master):
        self.master = master
        master.title("WT task kill & clear cache")
        master.minsize(400, 500)

        # Window Icon Setting
        if getattr(sys, 'frozen', False):
            # The application is packed (PyInstaller)
            icon_path = os.path.join(sys._MEIPASS, "myicon.ico") # Assuming the icon is named myicon.ico
        else:
            # Application started normally
            icon_path = "myicon.ico" # Or relative/absolute path

        try:
            master.iconbitmap(icon_path)
        except tk.TclError:
            print(f"Nie można załadować ikony z: {icon_path}")

        screen_width = master.winfo_screenwidth()
        screen_height = master.winfo_screenheight()
        window_width = 400
        window_height = 500
        center_x = int(screen_width / 2 - window_width / 2)
        center_y = int(screen_height / 2 - window_height / 2)
        master.geometry(f"{window_width}x{window_height}+{center_x}+{center_y}")

        # Automatically set system language
        system_lang_code = self.get_system_language()
        if system_lang_code not in ('en','ru','de','es','it','fr'):
            system_lang_code = 'pl'
        self.current_language_code = tk.StringVar(value=system_lang_code)
        self.current_language_name = tk.StringVar() # Store the language name

        # Load translations
        self.translations = self.load_translations()
        self.available_languages_codes = sorted(self.translations.keys())
        self.available_languages_names = [self.translations[code]['language'] for code in self.available_languages_codes]

        self.language_frame = ttk.LabelFrame(master, text=self._get_localized_message('select_language'))
        self.language_frame.pack(padx=10, pady=10)

        self.language_label = ttk.Label(self.language_frame, text=self._get_localized_message('language'))
        
        self.language_combo = ttk.Combobox(self.language_frame,
                                           textvariable=self.current_language_name,
                                           values=self.available_languages_names,
                                           state='readonly')
        self.language_combo.pack(side='left', padx=5)
        self.language_combo.bind('<<ComboboxSelected>>', self.on_language_change)

        self.action_frame = ttk.LabelFrame(master, text=self._get_localized_message('action_choice'))
        self.action_frame.pack(padx=10, pady=10)

        self.close_processes_button = ttk.Button(self.action_frame, text=self._get_localized_message('close_processes_option'), command=self.close_war_thunder_processes)
        self.close_processes_button.pack(pady=5, padx=10)

        self.delete_cache_button = ttk.Button(self.action_frame, text=self._get_localized_message('delete_cache_option'), command=self.browse_and_delete_cache)
        self.delete_cache_button.pack(pady=5, padx=10)

        self.info_label = ttk.Label(master, text="")
        self.info_label.pack(pady=5)

        self.log_text = scrolledtext.ScrolledText(master, height=10, state='disabled', wrap='word')
        self.log_text.pack(padx=10, pady=5, fill='both', expand=True)

        self.clear_log_button = ttk.Button(master, text=self._get_localized_message('clear_log'), command=self.clear_log)
        self.clear_log_button.pack(pady=(0, 10))

        self.close_button = ttk.Button(master, text=self._get_localized_message('close'), command=master.quit)
        self.close_button.pack(pady=5)

        # Set initial language name based on system language code AFTER UI elements are created
        self.set_language(system_lang_code)
        self.language_combo.set(self.translations[system_lang_code]['language'])

    def get_system_language(self):
        try:
            locale.setlocale(locale.LC_ALL, '')
            lang_code, _ = locale.getlocale()
            if lang_code:
                return lang_code.split('_')[0]
        except locale.Error:
            pass
        return 'en'

    def load_translations(self):
        return {
            'pl': {
                'language': "Polski",
                'select_language': "Wybierz język",
                'language_en': "English",
                'language_pl': "Polski",
                'action_choice': "Co chcesz zrobić?",
                'close_processes_option': "Zamknij procesy War Thunder",
                'delete_cache_option': "Usuń folder cache",
                'select_folder': "Wybierz główny folder War Thunder",
                'closing_processes': "Zamykanie procesów War Thunder...",
                'processes_closed': "Zamknięto {count} procesów War Thunder.",
                'no_processes_found': "Nie znaleziono procesów War Thunder do zamknięcia.",
                'confirm_delete_cache': "Czy na pewno chcesz usunąć folder 'cache'?",
                'cache_deleted': "Folder 'cache' został usunięty.",
                'cache_not_exists': "Folder 'cache' nie istnieje.",
                'delete_cache_error': "Wystąpił błąd podczas usuwania folderu 'cache'.",
                'delete_cache_cancelled': "Usunięcie folderu 'cache' zostało anulowane.",
                'finished': "Zakończono.",
                'close': "Zamknij",
                'clear_log': "Wyczyść log",
                'no_folder_selected': "Nie wybrano folderu."
            },
            'en': {
                'language': "English",
                'select_language': "Select language",
                'language_en': "English",
                'language_pl': "Polski",
                'action_choice': "What do you want to do?",
                'close_processes_option': "Terminate War Thunder processes",
                'delete_cache_option': "Delete cache folder",
                'select_folder': "Select the main War Thunder folder",
                'closing_processes': "Terminating War Thunder processes...",
                'processes_closed': "Terminated {count} War Thunder processes.",
                'no_processes_found': "No War Thunder processes found to terminate.",
                'confirm_delete_cache': "Are you sure you want to delete the 'cache' folder?",
                'cache_deleted': "The 'cache' folder has been deleted.",
                'cache_not_exists': "The 'cache' folder does not exist.",
                'delete_cache_error': "An error occurred while deleting the 'cache' folder.",
                'delete_cache_cancelled': "Deletion of the 'cache' folder has been cancelled.",
                'finished': "Finished.",
                'close': "Close",
                'clear_log': "Clear log",
                'no_folder_selected': "No folder was selected."
            },
            'de': {
                'language': "Deutch",
                'select_language': "Sprache auswählen",
                'language_en': "English",
                'language_pl': "Polski",
                'action_choice': "Was möchtest du tun?",
                'close_processes_option': "War Thunder Prozesse beenden",
                'delete_cache_option': "Cache-Ordner löschen",
                'select_folder': "Wähle den Hauptordner von War Thunder",
                'closing_processes': "Beende War Thunder Prozesse...",
                'processes_closed': "{count} War Thunder Prozesse beendet.",
                'no_processes_found': "Keine War Thunder Prozesse zum Beenden gefunden.",
                'confirm_delete_cache': "Bist du sicher, dass du den Ordner 'cache' löschen möchtest?",
                'cache_deleted': "Der Ordner 'cache' wurde gelöscht.",
                'cache_not_exists': "Der Ordner 'cache' existiert nicht.",
                'delete_cache_error': "Ein Fehler ist beim Löschen des Ordners 'cache' aufgetreten.",
                'delete_cache_cancelled': "Das Löschen des Ordners 'cache' wurde abgebrochen.",
                'finished': "Beendet.",
                'close': "Schließen",
                'clear_log': "Protokoll löschen",
                'no_folder_selected': "Kein Ordner ausgewählt."
            },
            'ru': {
                'language': "Русский",
                'select_language': "Выберите язык",
                'language_en': "English",
                'language_pl': "Polski",
                'action_choice': "Что вы хотите сделать?",
                'close_processes_option': "Завершить процессы War Thunder",
                'delete_cache_option': "Удалить папку 'cache'",
                'select_folder': "Выберите основную папку War Thunder",
                'closing_processes': "Завершение процессов War Thunder...",
                'processes_closed': "Завершено {count} процессов War Thunder.",
                'no_processes_found': "Не найдено процессов War Thunder для завершения.",
                'confirm_delete_cache': "Вы уверены, что хотите удалить папку 'cache'?",
                'cache_deleted': "Папка 'cache' была удалена.",
                'cache_not_exists': "Папка 'cache' не существует.",
                'delete_cache_error': "Произошла ошибка при удалении папки 'cache'.",
                'delete_cache_cancelled': "Удаление папки 'cache' было отменено.",
                'finished': "Завершено.",
                'close': "Закрыть",
                'clear_log': "Очистить журнал",
                'no_folder_selected': "Папка не выбрана."
            },
            'fr': {
                'language': "Français",
                'select_language': "Sélectionner la langue",
                'language_en': "English",
                'language_pl': "Polski",
                'action_choice': "Que voulez-vous faire ?",
                'close_processes_option': "Terminer les processus War Thunder",
                'delete_cache_option': "Supprimer le dossier 'cache'",
                'select_folder': "Sélectionner le dossier principal de War Thunder",
                'closing_processes': "Terminaison des processus War Thunder...",
                'processes_closed': "{count} processus War Thunder terminés.",
                'no_processes_found': "Aucun processus War Thunder à terminer n'a été trouvé.",
                'confirm_delete_cache': "Êtes-vous sûr de vouloir supprimer le dossier 'cache' ?",
                'cache_deleted': "Le dossier 'cache' a été supprimé.",
                'cache_not_exists': "Le dossier 'cache' n'existe pas.",
                'delete_cache_error': "Une erreur s'est produite lors de la suppression du dossier 'cache'.",
                'delete_cache_cancelled': "La suppression du dossier 'cache' a été annulée.",
                'finished': "Terminé.",
                'close': "Fermer",
                'clear_log': "Effacer le journal",
                'no_folder_selected': "Aucun dossier sélectionné."
            },
            'es': {
                'language': "Español",
                'select_language': "Seleccionar idioma",
                'language_en': "English",
                'language_pl': "Polski",
                'action_choice': "¿Qué quieres hacer?",
                'close_processes_option': "Terminar los procesos de War Thunder",
                'delete_cache_option': "Eliminar la carpeta 'cache'",
                'select_folder': "Seleccionar la carpeta principal de War Thunder",
                'closing_processes': "Terminando los procesos de War Thunder...",
                'processes_closed': "{count} procesos de War Thunder terminados.",
                'no_processes_found': "No se encontraron procesos de War Thunder para terminar.",
                'confirm_delete_cache': "¿Estás seguro de que quieres eliminar la carpeta 'cache'?",
                'cache_deleted': "La carpeta 'cache' ha sido eliminada.",
                'cache_not_exists': "La carpeta 'cache' no existe.",
                'delete_cache_error': "Ocurrió un error al eliminar la carpeta 'cache'.",
                'delete_cache_cancelled': "Se canceló la eliminación de la carpeta 'cache'.",
                'finished': "Finalizado.",
                'close': "Cerrar",
                'clear_log': "Borrar registro",
                'no_folder_selected': "No se seleccionó ninguna carpeta."
            },
            'it': {
                'language': "Italiano",
                'select_language': "Seleziona la lingua",
                'language_en': "English",
                'language_pl': "Polski",
                'action_choice': "Cosa vuoi fare?",
                'close_processes_option': "Termina i processi di War Thunder",
                'delete_cache_option': "Elimina la cartella 'cache'",
                'select_folder': "Seleziona la cartella principale di War Thunder",
                'closing_processes': "Terminazione dei processi di War Thunder...",
                'processes_closed': "{count} processi di War Thunder terminati.",
                'no_processes_found': "Nessun processo di War Thunder trovato da terminare.",
                'confirm_delete_cache': "Sei sicuro di voler eliminare la cartella 'cache'?",
                'cache_deleted': "La cartella 'cache' è stata eliminata.",
                'cache_not_exists': "La cartella 'cache' non esiste.",
                'delete_cache_error': "Si è verificato un errore durante l'eliminazione della cartella 'cache'.",
                'delete_cache_cancelled': "L'eliminazione della cartella 'cache' è stata annullata.",
                'finished': "Finito.",
                'close': "Chiudi",
                'clear_log': "Cancella registro",
                'no_folder_selected': "Nessuna cartella selezionata."
            }
        }

    def _get_localized_message(self, key, format_args=None):
        """Internal function to get localized message based on current language code."""
        lang_code = self.current_language_code.get()
        message = self.translations.get(lang_code, self.translations['en']).get(key, f"[{key}]")
        if format_args:
            return message.format(**format_args)
        return message

    def get_localized_message(self, key, format_args=None):
        """Public function to get localized message."""
        lang_name = self.current_language_name.get()
        # Find the language code based on the selected name
        lang_code_list = [code for code, data in self.translations.items() if data['language'] == lang_name]
        if lang_code_list:
            lang_code = lang_code_list[0]
            message = self.translations.get(lang_code, self.translations['en']).get(key, f"[{key}]")
            if format_args:
                return message.format(**format_args)
            return message
        else:
            # Fallback if the language name doesn't match (shouldn't happen if combo is set correctly)
            return f"[{key}]"

    def set_language(self, lang_code):
        if lang_code in self.translations:
            self.current_language_code.set(lang_code)
            self.current_language_name.set(self.translations[lang_code]['language'])
            self.master.title("WT task kill & clear cache")
            self.language_frame.config(text=self._get_localized_message('select_language'))
            self.language_label.config(text=self._get_localized_message('language'))
            self.action_frame.config(text=self._get_localized_message('action_choice'))
            self.close_processes_button.config(text=self._get_localized_message('close_processes_option'))
            self.delete_cache_button.config(text=self._get_localized_message('delete_cache_option'))
            self.close_button.config(text=self._get_localized_message('close'))
            self.clear_log_button.config(text=self._get_localized_message('clear_log'))

    def on_language_change(self, event):
        selected_language_name = self.current_language_name.get()
        # Find the language code based on the selected name
        lang_code_list = [code for code, data in self.translations.items() if data['language'] == selected_language_name]
        if lang_code_list:
            self.set_language(lang_code_list[0])

    def log_message(self, message):
        self.log_text.configure(state='normal')
        self.log_text.insert('end', message + '\n')
        self.log_text.see('end')
        self.log_text.configure(state='disabled')

    def clear_log(self):
        self.log_text.configure(state='normal')
        self.log_text.delete('1.0', 'end')
        self.log_text.configure(state='disabled')

    def close_war_thunder_processes(self):
        self.info_label.config(text=self._get_localized_message('closing_processes'))
        self.log_message(self._get_localized_message('closing_processes'))
        processes_to_kill = [
            "launcher.exe", "aces.exe", "aces-min-cpu.exe", "cefprocess.exe", "gjagent.exe",
            "aces_be.exe", "beac_wt_mlauncher.exe", "beservice.exe", "beservice_x64.exe",
            "eac_wt_mlauncher.exe", "easyanticheat.exe", "easyanticheat_setup.exe", "eac_launcher.exe"
        ]
        total_killed = 0
        for proc_name in processes_to_kill:
            killed = self.kill_process_by_name(proc_name)
            total_killed += killed

        if total_killed > 0:
            msg = self._get_localized_message('processes_closed', {'count': total_killed})
        else:
            msg = self._get_localized_message('no_processes_found')

        self.info_label.config(text=msg)
        self.log_message(msg)

    def kill_process_by_name(self, process_name):
        killed_count = 0
        for proc in psutil.process_iter(['pid', 'name']):
            if proc.info['name'].lower() == process_name.lower():
                try:
                    psutil.Process(proc.info['pid']).terminate()
                    killed_count += 1
                    self.log_message(f"✔ Terminated {process_name} (PID: {proc.info['pid']})")
                except Exception as e:
                    self.log_message(f"⚠ Failed to terminate {process_name} (PID: {proc.info['pid']}): {e}")
        return killed_count

    def browse_and_delete_cache(self):
        base_folder = filedialog.askdirectory(title=self._get_localized_message('select_folder'))
        if base_folder:
            cache_folder_path = os.path.join(base_folder, "cache")
            if not os.path.exists(cache_folder_path):
                msg = self._get_localized_message('cache_not_exists')
                self.info_label.config(text=msg)
                self.log_message(msg)
            else:
                confirm = messagebox.askyesno(self._get_localized_message('confirm_delete_cache'), self._get_localized_message('confirm_delete_cache'))
                if confirm:
                    try:
                        shutil.rmtree(cache_folder_path)
                        msg = self._get_localized_message('cache_deleted')
                        self.info_label.config(text=msg)
                        self.log_message(f"✔ {msg} ({cache_folder_path})")
                    except Exception as e:
                        msg = self._get_localized_message('delete_cache_error')
                        self.info_label.config(text=msg)
                        self.log_message(f"⚠ {msg}: {e}")
                else:
                    msg = self._get_localized_message('delete_cache_cancelled')
                    self.info_label.config(text=msg)
                    self.log_message(msg)
        else:
            msg = self._get_localized_message('no_folder_selected')
            self.info_label.config(text=msg)
            self.log_message(msg)

if __name__ == "__main__":
    root = tk.Tk()
    app = WTTaskCacheGUI(root)
    root.mainloop()
