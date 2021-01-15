/*
 * dataType.h
 *
 *  Created on: Aug 6, 2020
 *      Author: CongThuan
 */

#ifndef DATATYPE_H_
#define DATATYPE_H_

#define MAX_QUEUE 1000
#define UK43DATALENGTH 398
#define UK61DATALENGTH 416
#define UK51DATALENGTH 124
#define UK31DATALENGTH 1432
enum PhuongPhapDan {
	ChuaXacDinhPP = 0,
	KDU = 1,
	MTT = 2
};
enum TrangThai {
	Disable = 0,
	Enable = 1
};
enum KenhQuanSat {
	ChuaXacDinhKenhQuanSat = 0,
	PNK = 1,			// Vo tuyen
	OEK = 2				// QuangDienTu
};
enum Ratio {
	M40 = 0,
	M55 = 1,
	M80 = 2,
	M100 = 3
};
enum CheDo {
	CheDoChienDau = 0,
	CheDoKiemTraChucNang = 1,
	CheDoLuyenTap = 2,
	ChuaXacDinhCheDo = 3
};
enum CheDoPhong {
	PhongPhatMot = 1,
	PhongHangLoat = 0
};
enum PhuongPhapTinhTenLua{
	Mode1 = 0,
	Mode2 = 1
};
enum TrangThaiTenLua {
	NotReady = 0,
	Ready = 1
};
enum XacDinhTenLua {
	NotAvaiable = 0,
	Avaiable = 1
};
enum CheDoBamSat{
	ChuaXacDinh = 0,
	BamSatTuDong = 1,
	BamSatBangTay = 2
};
typedef struct __attribute__(( packed)) Rocket {
	bool TrangThai;		//TrangThaiTenLua TrangThai;
	bool Rocket_avai;	//XacDinhTenLua Rocket_avai;
} Rocket;

typedef struct __attribute__(( packed)) uk43DataStruct {
	int16_t r;
	int16_t v;
	int16_t p;
	int16_t h;

	int16_t h1_k1;
	int16_t h2_k1;
	int16_t h1_k2;
	int16_t h2_k2;
	int16_t d_k1;
	int16_t d_k2;

	int8_t app;
	int8_t kdu_mtt_1_pnk_oek_1;  	// sua
	int8_t kdu_mtt_2;			// sua
	int8_t pnk_oek_2;			// sua

} uk43DataStruct;

typedef struct __attribute__(( packed)) uk61DataStruct {
	double b;
	double e;
	int16_t b1;
	int16_t b2;
	int16_t b3;
	int16_t b4;
	int8_t launch_mode;
	int8_t EMNR;
} uk61DataStruct;

typedef struct __attribute__(( packed)) uk31DataStruct {
	int8_t ratio;
	int8_t mode;
	int8_t cau_mb;
	int8_t mb1_mb2;
	int8_t oek;
	int8_t yhb;
	int8_t tt;
	int8_t numberRocketEncode;
//	CheDoBamSat BamSat; 			// 1 = Tu dong, 2 = Bang Tay, 0 = chuaXacDinh
	bool BamSat; 			// 1 = Tu dong, 2 = Bang Tay, 0 = chuaXacDinh
} uk31DataStruct;

typedef struct __attribute__(( packed)) uk51DataStruct {
	int8_t numberRocketEncode;
} uk51DataStruct;

typedef struct ukDataSend {

	char header[4];				// header = 125Mon
	int16_t length;					// length = sizeof (ukDataSend)

	int16_t r;						// Cu ly = r*1.5
	int16_t v;						// Van toc muc tieu
	int16_t p;						// Khoang cach tam dai toi hinh chieu duong bay cua muc tieu
	int16_t h;						// Do cao cua muc tieu

	int16_t h1_k1;					// sai so h1 kenh k1
	int16_t h2_k1;					// sai so h2 kenh k1
	int16_t h1_k2;					// sai so h1 kenh k2
	int16_t h2_k2;					// sai so h2 kenh k2
	int16_t d_k1;					// khoang cah tu dai den ten lua o kenh 1 = d_k1 * 1.5
	int16_t d_k2;					// khoang cah tu dai den ten lua o kenh 2 = d_k2 * 1.5
//26
	bool app;						//TrangThai app;
	PhuongPhapDan ppd_k1;			// Phuong phap dan - kenh 1
	PhuongPhapDan ppd_k2;			// Phuong phap dan kenh 2
	KenhQuanSat 	kqs_k1;
	KenhQuanSat		kqs_k2;
	double b;						// Phuong vi
	double e;						// Goc ta
//59
	Rocket b1_1;					// Trang thai ten lua be
	Rocket b1_2;					// Trang thai ten lua be 1 - qua 2
	Rocket b1_3;					// Trang thai ten lua be 1 - qua 3
	Rocket b1_4;					// Trang thai ten lua be 1 - qua 4

	Rocket b2_1;
	Rocket b2_2;
	Rocket b2_3;
	Rocket b2_4;

	Rocket b3_1;
	Rocket b3_2;
	Rocket b3_3;
	Rocket b3_4;

	Rocket b4_1;					// Trang thai ten lua be 4 - qua 1
	Rocket b4_2;
	Rocket b4_3;
	Rocket b4_4;

	bool launch_mode;			// CheDoPhong launch_mode;
	bool EMNR;					//TrangThai EMNR;

	Ratio ratio;
	CheDo mode;
	bool cau; 					//TrangThai cau;
	bool mb_enable;				//	TrangThai mb_enable;
	bool mb1;					//	TrangThai mb1;
	bool mb2;					//	TrangThai mb2;
	bool oek;					//	TrangThai oek;
	bool yhb;					//	TrangThai yhb;
	bool tt;					//	TrangThai tt;
	PhuongPhapTinhTenLua numberRocketEncode;
//	CheDoBamSat BamSat;			// Che do bam sat: 1 = Tu dong, 2 = Bang Tay, 0 = chuaXacDinh //bo xung
	bool BamSat;				// Che do bam sat: 1 = Tu dong, 0 = Bang tay //bo xung
} __attribute__(( packed)) ukDataSend;

#endif /* DATATYPE_H_ */
