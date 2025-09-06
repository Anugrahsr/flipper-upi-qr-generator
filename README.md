# 🏦 UPI QR Generator for Flipper Zero

<div align="center">

[![Flipper Zero](https://img.shields.io/badge/Flipper%20Zero-Compatible-orange)](https://flipperzero.one)
[![UPI](https://img.shields.io/badge/UPI-India-blue)](https://en.wikipedia.org/wiki/Unified_Payments_Interface)
[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](https://choosealicense.com/licenses/mit/)
[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen)](https://github.com/your-repo)

*Transform your Flipper Zero into a powerful UPI payment QR code generator*

[Features](#-features) •
[Installation](#-installation) •
[Usage](#-usage) •
[Building](#-building-from-source) •
[Technical Details](#-technical-details)

</div>

---

## 📱 Screenshots

<table>
  <tr>
    <td align="center">
      <img src="apps.png" width="300" alt="Main Interface"/>
      <br/>
      <sub><b>Main Application Interface</b></sub>
      <br/>
      <sub>Clean UPI ID input screen</sub>
    </td>
    <td align="center">
      <img src="about.png" width="300" alt="About Screen"/>
      <br/>
      <sub><b>Application Details</b></sub>
      <br/>
      <sub>Information and app details</sub>
    </td>
  </tr>
</table>

## ✨ Features

🎯 **Easy QR Generation** - Generate UPI QR codes with custom payee information  
🏪 **Bank Format Support** - Full support for `username@bank` format input  
📱 **Fullscreen Display** - Large QR code display optimized for mobile scanning  
💾 **Save Functionality** - Store generated QR codes directly to device storage  
🎨 **Clean Interface** - Intuitive and user-friendly design  
⚡ **Fast Performance** - Quick QR code generation and display  

## 🚀 Installation

### Method 1: Direct Installation
```bash
# Copy the application file to your Flipper Zero
cp upi_qr.fap /path/to/flipper/apps/
```

### Method 2: Using qFlipper
1. Connect your Flipper Zero to your computer
2. Open qFlipper application
3. Navigate to the `apps` folder
4. Drag and drop `upi_qr.fap` into the folder

## 🛠️ Building from Source

### Prerequisites
- Python 3.7+
- ufbt (Flipper Zero build tool)

### Build Steps
```bash
# Install the Flipper Zero build tool
pip install ufbt

# Clone the repository (if applicable)
git clone <your-repo-url>
cd upi-qr-generator

# Build the application
ufbt

# Launch on connected Flipper Zero (optional)
ufbt launch
```

### Development Commands
```bash
# Clean build artifacts
ufbt clean

# Build in debug mode
ufbt debug

# Format code
ufbt format
```

## 📖 Usage

### Step-by-Step Guide

1. **Launch Application**
   - Navigate to `Apps` → `Tools` → `UPI QR Generator`

2. **Enter UPI Details**
   - Input your UPI ID (e.g., `john.doe@paytm`)
   - Enter the payee name for the transaction

3. **Generate QR Code**
   - The app will automatically generate a QR code
   - QR code follows standard UPI payment format

4. **View & Save**
   - Press `OK` to view in fullscreen mode
   - Use the save option to store the QR code
   - Share with others for easy payments

### Supported UPI ID Formats
- `username@paytm`
- `mobilenumber@upi`
- `username@googlepay`
- `username@phonepe`
- And all other UPI-compatible formats

## 🔧 Technical Details

### QR Code Specifications
| Parameter | Value |
|-----------|-------|
| **Version** | QR Code Version 3 |
| **Error Correction** | ECC_LOW |
| **Format** | `upi://pay?pa=<UPI_ID>&pn=<PAYEE_NAME>` |
| **Encoding** | UTF-8 with URL encoding |

### UPI Payment String Format
```
upi://pay?pa=<UPI_ID>&pn=<PAYEE_NAME>
```

### Features Implementation
- ✅ **URL Encoding** - Proper handling of special characters in payee names
- ✅ **Error Handling** - Graceful handling of invalid UPI IDs
- ✅ **Memory Management** - Efficient memory usage for QR generation
- ✅ **Display Optimization** - Optimized for Flipper Zero's screen resolution

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2024 UPI QR Generator Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction...
```

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

### Development Setup
1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📞 Support

- 🐛 **Bug Reports**: [GitHub Issues](https://github.com/your-repo/issues)
- 💡 **Feature Requests**: [GitHub Discussions](https://github.com/your-repo/discussions)
- 📖 **Documentation**: [Wiki](https://github.com/your-repo/wiki)

## 🌟 Acknowledgments

- [Flipper Zero Team](https://flipperzero.one) for the amazing hardware platform
- [UPI Ecosystem](https://www.npci.org.in/what-we-do/upi) for revolutionizing digital payments in India
- All contributors who helped make this project better

---

<div align="center">

**Made with ❤️ for the Flipper Zero community**

⭐ Star this project • 🔔 Watch for updates • 🍴 Fork and contribute

</div>
