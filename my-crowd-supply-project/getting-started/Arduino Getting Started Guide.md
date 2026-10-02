# 🛸 How to Setup Your Hyper-S3-Rocket board

Follow these **3 easy steps** so you can upload your code the easy way!

---

### ⚡ Step 1: The Magic Button Trick
Before changing any computer settings, we need to wake up the Rocket's hidden programming mode so your computer can recognize it.

1. **Plug your Rocket board** into your computer with a USB-C cable.
2. Press and **HOLD the BOOT button** on your board and...
3. Tap the **RESET button** once, then **let go of the BOOT button**. 

---

### 📦 Step 2: Click the Board Settings
Open the **Tools** menu at the top of your Arduino IDE screen and match these choices exactly:

* 🎛️ **Board:** ➔ `esp32` ➔ **`ESP32S3 Dev Module`** Need 💡 Look on.
* ⚡ **USB CDC On Boot:** ➔ **`Enabled`**
* 🔌 **USB Mode:** ➔ **`Hardware CDC and JTAG`**
* 📦 **Upload Mode:** ➔ **`TinyUSB`**
* 💾 **PSRAM:** ➔ **`QSPI PSRAM`**
* 📟 **Port:** ➔ Choose the one that now says **`ESP32 Family Device`**!

> 💡 **Don't see the `esp32` option?**  
> Don't worry! Go to **Tools** ➔ **Board** ➔ **Boards Manager...** right at the top of the menu. Type **esp32** into the search bar, find the one by **Espressif Systems**, and click the blue **Install** button. Once it finishes downloading, your esp32 option will appear in the menu! Then select the **ESP32S3 Dev Module**.

---

### 🎉 Step 3: Flash and Play!
1. Click **Upload (➔)**.
2. When the console reads **"Hard resetting via ..."**, press the **RESET button** once on your board.
3. Open the Serial Monitor to view output.

---

### ⚠️ Two Golden Rules for the Serial Monitor
* 🔒 **Close BEFORE you Upload:** Close the Serial Monitor before clicking Upload to prevent port lockouts.
* 🛰️ **Open to Unblock the Clock:** Press **Ctrl + Shift + M** to flush the channel if any timing becomes erratic.

**That's it!** No need to toggle the buttons ever again. Rocket knows exactly what to do. Uploading new code is fun, easy, and completely hands-free! 🚀✨
