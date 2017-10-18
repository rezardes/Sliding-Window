# TUGAS BESAR 1 JARINGAN KOMPUTER

### A. Petunjuk Penggunaan Program
1) Buka terminal di linux
2) Jalankan makefile dengan perintah:
3) Ketikkan ./send <filename> <windowsize> <buffersize> <destination_ip> <destination_port> dan ./receive <filename> <windowsize> <buffersize> <port> pada 2 terminal berbeda agar program dapat dijalankan. Isi "<>" sesuai dengan keterangannya.

### B. Cara Kerja Sliding Window
Sliding window flow control dirancang untuk membenahi sistem flow control stop and wait. Pada stop and wait flow control seolah-olah blok-blok data dikirimkan satu persatu dan mengirimkannya menunggu balasan jika blok data tersebut sudah sampai di receiver dan receiver sudah memberikan balasan. Berbeda pada sliding window, transmitter dapat mengirimkan blok-blok frame lebih banyak lalu setelah beberapa frame telah terkirim, barulah receiver memberikan balasan. Pada sliding window tiap-tiap blok frame diberi nomor.

Sliding window flow control jauh lebih efisien dibandingkan dengan stop and wait karena:
- Dapat mengirimkan lebih dari satu blok frame
- Waktu penundaan/delay lebih sedikit
- Transfer data menjadi lebih cepat

Dalam pengaplikasiannya, sliding window sangat dibutuhkan dalam komunikasi data karena memiliki nilai efisiensi yang tinggi.

Data dikirimkan dalam bentuk frame yang memiliki SeqNum (sequence number) atau nomor urut data. ACK  dikirimkan oleh receiver (penerima) sebagai konfirmasi bahwa frame dengan SeqNum tertentu sudah diterima. Sender (pengirim) menyimpan 3 variabel, yaitu SWS (send window size), LFS (last frame sent), dan LAR (last ACK received). Ketiga variabel ini harus memenuhi constraint: LFS - LAR <= SWS. Pada sisi receiver, disimpan juga 3 variabel, yaitu RWS (receiver window size), LFR (last frame received), dan LAF (last acceptable frame). Ketiga variabel tersebut harus memenuhi constraint: LAF - LFR <= RWS. Ketika sebuah frame sampai pada receiver, akan dilakukan pengecekan apakah SeqNum frame tersebut memenuhi: LFR < SeqNum <= LAR. Jika iya, maka frame diterima dan jika tidak, maka frame ditolak.

### C. Pembagian Tugas
| No. | Nama (NIM) | Pembagian Tugas |
| --- | ---------- | --------------- |
| 1. | M. Dicky Andika Putra (13515044) |  |
| 2. | Muthmainnah (13515059) |  |
| 3. | M. Treza Nolandra (13515080) |  |

### D. Pertanyaan & Jawaban
1. Apa yang terjadi jika advertised window yang dikirim bernilai 0? Apa cara untuk menangani hal tersebut?
Jawab :
Advertise window adalah jumlah sisa window size yang bisa diisi pada receiver side. Maka dari itu, jika advertised window diketahui nilainya 0, maka sender harus menunggu jika ingin mengirim frame selanjutnya, sampai sender menerima ACK yang menginformasikan bahwa advertise window bernilai lebih dari 0.

2. Sebutkan field data yang terdapat TCP Header serta ukurannya, ilustrasikan, dan jelaskan kegunaan dari masing-masing field data tersebut!
Jawab :
Berikut ilustrasi TCP Header :
![](/tcp-header-56a1adc85f9b58b7d0c1a24f.png)


- Source Port dan Destination Port 
Mengidentifikasikan source port dan destination port. Kedua fields ini jika digabungkan dengan source IP address dan destination IP address mengidentifikan sebuah TCP connection.

- Sequence Number
Digunakan untuk menandai urutan message pada kumpulan messages. 

- Acknowledgment Number
Digunakan untuk komunikasi antara sender dan receiver. Acknowledgment number mewakili sequence number message yang baru saja diterima atau message yang diharapkan diterima.

- Header Length (Offset)
Offset mewakili ukuran TCP header jika dikalikan dengan 4 bytes. Contohnya, TCP header yang tidak menggunakan optional field memiliki offset 5 (mewakili 20 bytes).

- Reserved Data (0)
Reserved data selalu bernilai 0. Field ini ada agar total header size merupakan nilai kelipatan 4. Hal ini bertujuan untuk efisiensi pada computer data processing.

- Flags
Field ini terdiri dari 9 bits, yaitu 6 standard dan 3 extended control flags. Setiap bit-nya merepresentasikan on atau off yang digunakan untuk mengatur data flow pada kondisi tertentu. Flag yang termasuk diantaranya SYN, FIN, RESET, PUSH, URG, dan ACK. 

- Advertised Window
Field ini digunakan untuk mengetahui window size yang tersedia untuk menampung data yang akan dikirim oleh sender.

- Checksum
Checksum adalah nilai yang di-generate oleh protocol sender sebagai teknik matematika untuk mendeteksi apabila terjadi error pada pengiriman data yang menyebabkan file corrupted.

- Urgent Pointer
Urgent pointer seringkali bernilai 0 atau diabaikan. Urgent pointer digunakan untuk menandai bagian dari TCP header yang urgent (penting) sebagai requiring priority processing. Urgent pointer mengidentifikasikan dimana bagian nonurgent pada header dimulai.

- Optional Data
Sebagai field untuk menyimpan data optional, termasuk special acknowledgement support dan window scaling algorithm.
