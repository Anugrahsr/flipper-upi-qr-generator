# UPI QR Generator for Flipper Zero

A Flipper Zero application that generates UPI (Unified Payments Interface) QR codes for easy digital payments in India.

## Screenshots

![Application Interface](apps.png)
*Main application interface showing UPI ID input*

![About Screen](about.png)
*Application information and details*

## Features

- Generate UPI QR codes with custom payee information
- Support for username@bank format input
- Fullscreen QR code display for easy scanning
- Save QR codes to device storage
- Clean and intuitive user interface

## Installation

1. Copy the `upi_qr.fap` file to your Flipper Zero's `apps` folder
2. Or build from source using `ufbt`

## Building from Source

```bash
# Install ufbt (Flipper Zero build tool)
pip install ufbt

# Build the application
ufbt

# Launch on connected Flipper Zero
ufbt launch
```

## Usage

1. Open the UPI QR Generator app on your Flipper Zero
2. Enter your UPI ID (e.g., username@bank)
3. Enter the payee name
4. View the generated QR code
5. Press OK to view fullscreen or save the QR code

## Technical Details

- Uses QR Code version 3 with ECC_LOW error correction
- Supports UPI payment string format: `upi://pay?pa=<UPI_ID>&pn=<PAYEE_NAME>`
- Proper URL encoding for special characters in payee names

## License

This project is open source and available under the MIT License.
