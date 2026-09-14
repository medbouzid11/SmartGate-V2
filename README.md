# 🚪 SmartGate V2 - Contrôle de Portail RF 438.9MHz & Interface Web ESP32

[![ESP32](https://img.shields.io/badge/Hardware-ESP32-blue.svg)](https://www.espressif.com/)
[![License-MIT](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)

Système embarqué complet basé sur un **ESP32** permettant de piloter un moteur de portail coulissant (compatible carte **AC101**) via deux modes de commande simultanés :
1. **Réception Radio RF 438.9MHz :** Décodage du clavier à code sans fil **V2 SIRMO-DIGIT** (Protocole 53200).
2. **Interface Web Moderne (PWA/Mobile) :** Contrôle local sans connexion Internet via un point d'accès Wi-Fi (Access Point).

---

## 🌟 Fonctionnalités Principales

* 📡 **Décodage RF 53200 (24-bit) :** Interception et validation précise de la trame binaire du clavier V2 (`100110011001100110011010`) avec filtrage avancé du bruit radioélectrique.
* 📶 **Point d'Accès Wi-Fi Autonome (Offline) :** Génération d'un réseau Wi-Fi local dédié sur l'ESP32 (`192.168.4.1`), idéal pour les moteurs situés hors de portée du réseau domestique.
* 📱 **Interface Web Sombre & Moderne :** Page de commande épurée "Welcome to House Garbaa" adaptée aux écrans de smartphones.
* ⚙️ **Gestion du Wi-Fi via l'Interface :** Icône de paramètres permettant de modifier le nom du réseau (SSID) et le mot de passe directement depuis le navigateur (sauvegarde en mémoire NVS/Preferences).
* ⚡ **Contact Sec Relais :** Génération d'une impulsion de 1 seconde sur le bornier **START** du moteur.

---

## 🛠️ Matériel Requis

* **ESP32 DevKit V1**
* **Récepteur RF SRX882** (433.92 / 438.9 MHz)
* **Module Relais 5V / 3.3V** (Optocouplé)
* **Clavier à code RF V2 SIRMO-DIGIT**
* **Moteur de portail coulissant** (ex: Carte de commande AC101)

---

## 🔌 Schéma de Câblage

### 1. ESP32 <-> Récepteur SRX882
* `VCC` $\rightarrow$ **3.3V / 5V** (ESP32)
* `GND` $\rightarrow$ **GND** (ESP32)
* `DATA` $\rightarrow$ **GPIO 13** (ESP32) *(Resistor Pull-down conseillé)*

### 2. ESP32 <-> Module Relais
* `VCC` $\rightarrow$ **5V**
* `GND` $\rightarrow$ **GND**
* `IN` $\rightarrow$ **GPIO 23** (ESP32)

### 3. Relais <-> Carte Moteur AC101
* `COM` (Relais) $\rightarrow$ Borne **GND / COM** (AC101)
* `NO` (Relais) $\rightarrow$ Borne **START / PED** (AC101)

---

## 🚀 Installation & Déploiement

1. Ouvrez le projet dans l'**IDE Arduino** ou **VS Code avec PlatformIO**.
2. Installez les bibliothèques intégrées au cœur ESP32 (`WiFi.h`, `WebServer.h`, `Preferences.h`).
3. Téléversez le code sur votre **ESP32**.
4. Connectez-vous au réseau Wi-Fi émis par le module :
   * **SSID par défaut :** `Smart_Gate_V2`
   * **Mot de passe :** `gate12345678`
5. Ouvrez votre navigateur mobile à l'adresse : `http://192.168.4.1`

---

## 📱 Aperçu de l'Interface Web

```text
+---------------------------------------+
| ⚙️                                    |
|         WELCOME TO HOUSE GARBAA       |
|          AUTOMATIC GATE CONTROL       |
|                                       |
|               (  OPEN  )              |
|                                       |
| Realized by Bouzid Aluminium &        |
| Automatsation                         |
+---------------------------------------+# SmartGate-V2
