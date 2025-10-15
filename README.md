# Blood-Donation-and-Receiver-mangement-System-using-C-programming-


🩸 Blood Donor & Receiver Management System

A desktop application developed using C and GTK+ 3 for managing blood donors and receivers, tracking available blood stock, and searching for donors by blood group.

📌 Features

🔐 Login System

🧑‍🤝‍🧑 Add/View Donors and Receivers

🩸 Check Blood Stock by Blood Group

🧬 Find Donor by Blood Group

💾 CSV File Storage for Data Persistence

🖥️ GUI Tech Stack

Language: C

Toolkit: GTK+ 3

Build System: gcc

📷 Screenshots (Optional Section)

<img width="489" height="529" alt="image" src="https://github.com/user-attachments/assets/abac96ad-4259-4f59-92a3-4686a600946d" />

```markdown
📁 Project Structure
project/
├── app.c                 # Main application code
├── donors.csv             # Stores donor data
├── receivers.csv          # Stores receiver data
├── b.jpg                  # Background image for login screen
└── README.md              # Project documentation (this file)

🛠️ How to Build & Run
🧰 Prerequisites

Linux OS or WSL (GTK+ is best supported on Linux)

GTK+ 3 development libraries
Install with:

sudo apt update
sudo apt install libgtk-3-dev

🔧 Build Instructions
gcc 'pkg-config --cflags gtk+-3.0' -o app app.c `pkg-config 'pkg-config --libs gtk+-3.0'

▶️ Run the Application
./app

🔑 Default Credentials
Username	Password
sumanyu	7689
🗃️ Data Storage Format

Donor and receiver data is stored in CSV format:

donors.csv / receivers.csv

Name,Age,BloodGroup,Contact

💡 Functional Highlights

Login Authentication: Basic credential check (can be extended).

Donor/Receiver Registration: Adds user info to CSV file.

List Views: Displays all or filtered data in a GTK TreeView.

Search Donors: Filter by blood group.

Check Blood Stock: Aggregates count by blood group.

Logout Functionality: Return to login screen from main menu.

🚀 Future Improvements (Suggestions)

Replace file-based storage with SQLite or MySQL.

Add user registration and role-based access.

Export reports.

Integrate email/SMS alerts.

Add search by location/availability.


👨‍💻 Author

Sumanyu
Feel free to contribute or fork this project!
