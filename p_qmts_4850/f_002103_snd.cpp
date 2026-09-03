/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		wsl
Version:    1.0
Date:		2024-3-11
Description:检验批发送（北）
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

int f_002103_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString tableName = " ";
	CString tableName1 = " ";
	CString tableName2 = " ";
	CString msg_type = " ";
	CString fr1 = " ";
	CString fr2 = " ";
	CString fr3 = " ";
	CString fr4 = " ";
	CString fr5 = " ";
	CString in_name = " ";
	CString	in_rout1 = " ";
	CString	in_rout2 = " ";
	CString	out_name = " ";
	CString	out_rout1 = " ";
	CString	out_rout2 = " ";
	CString primaryKey = " ";
	CString primaryData = " ";
	CString tc_no = "002103";

	EPEX epex;
	try
	{
		//获取输入参数
		blkNum = bcls_rec->Tables.IndexOf("QMTSJY");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MMLCSND 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (bcls_rec->Tables["QMTSJY"].Columns.Contains("TABLE_NAME"))
			tableName = bcls_rec->Tables["QMTSJY"].Rows[0]["TABLE_NAME"].ToString().Trim();
		if (bcls_rec->Tables["QMTSJY"].Columns.Contains("TABLE_NAME1"))
			tableName1 = bcls_rec->Tables["QMTSJY"].Rows[0]["TABLE_NAME1"].ToString().Trim();
		if (bcls_rec->Tables["QMTSJY"].Columns.Contains("TABLE_NAME2"))
			tableName2 = bcls_rec->Tables["QMTSJY"].Rows[0]["TABLE_NAME2"].ToString().Trim();


		if (bcls_rec->Tables["QMTSJY"].Columns.Contains("PRIMARY_KEY"))
			primaryKey = bcls_rec->Tables["QMTSJY"].Rows[0]["PRIMARY_KEY"].ToString().Trim();
		if (bcls_rec->Tables["QMTSJY"].Columns.Contains("PRIMARY_DATA"))
			primaryData = bcls_rec->Tables["QMTSJY"].Rows[0]["PRIMARY_DATA"].ToString().Trim();
		if (bcls_rec->Tables["QMTSJY"].Columns.Contains("MSGTYPE"))
			msg_type = bcls_rec->Tables["QMTSJY"].Rows[0]["MSGTYPE"].ToString().Trim();
		if (bcls_rec->Tables["QMTSJY"].Columns.Contains("FREESEL1"))
			fr1 = bcls_rec->Tables["QMTSJY"].Rows[0]["FREESEL1"].ToString().Trim();
		if (bcls_rec->Tables["QMTSJY"].Columns.Contains("FREESEL2"))
			fr2 = bcls_rec->Tables["QMTSJY"].Rows[0]["FREESEL2"].ToString().Trim();
		if (bcls_rec->Tables["QMTSJY"].Columns.Contains("FREESEL3"))
			fr3 = bcls_rec->Tables["QMTSJY"].Rows[0]["FREESEL3"].ToString().Trim();
		if (bcls_rec->Tables["QMTSJY"].Columns.Contains("FREESEL4"))
			fr4 = bcls_rec->Tables["QMTSJY"].Rows[0]["FREESEL4"].ToString().Trim();
		if (bcls_rec->Tables["QMTSJY"].Columns.Contains("FREESEL5"))
			fr5 = bcls_rec->Tables["QMTSJY"].Rows[0]["FREESEL5"].ToString().Trim();
		if (bcls_rec->Tables["QMTSJY"].Columns.Contains("IN_NAME"))
			in_name = bcls_rec->Tables["QMTSJY"].Rows[0]["IN_NAME"].ToString().Trim();
		if (bcls_rec->Tables["QMTSJY"].Columns.Contains("IN_ROUT1"))
			in_rout1 = bcls_rec->Tables["QMTSJY"].Rows[0]["IN_ROUT1"].ToString().Trim();
		if (bcls_rec->Tables["QMTSJY"].Columns.Contains("IN_ROUT2"))
			in_rout2 = bcls_rec->Tables["QMTSJY"].Rows[0]["IN_ROUT2"].ToString().Trim();
		if (bcls_rec->Tables["QMTSJY"].Columns.Contains("OUT_NAME"))
			out_name = bcls_rec->Tables["QMTSJY"].Rows[0]["OUT_NAME"].ToString().Trim();
		if (bcls_rec->Tables["QMTSJY"].Columns.Contains("OUT_ROUT1"))
			out_rout1 = bcls_rec->Tables["QMTSJY"].Rows[0]["OUT_ROUT1"].ToString().Trim();
		if (bcls_rec->Tables["QMTSJY"].Columns.Contains("OUT_ROUT2"))
			out_rout2 = bcls_rec->Tables["QMTSJY"].Rows[0]["OUT_ROUT2"].ToString().Trim();



		if ("" == tableName)
		{
			strcpy(s.msg, "传入表名为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if ("" == tableName1)
		{
			strcpy(s.msg, "传入表名为空1。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if ("" == tableName2)
		{
			strcpy(s.msg, "传入表名为空2。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if ("" == primaryKey)
		{
			strcpy(s.msg, "传入主键为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		Log::Trace("", __FUNCTION__, "===tableName= [{0}]", tableName);
		CModel tqmts2103(tableName);
		CModel tqmts2131(tableName1);
		CModel tqmts2132(tableName2);


		/* 查询数据 */
		tqmts2103[primaryKey] = primaryData;
		tqmts2103.Query(primaryKey);
		tqmts2103.TrimOrBlank();


		tqmts2131[primaryKey] = primaryData;
		tqmts2131.Query(primaryKey);
		tqmts2131.TrimOrBlank();

		tqmts2132[primaryKey] = primaryData;
		tqmts2132.Query(primaryKey);
		tqmts2132.TrimOrBlank();


		if (epex.Initialize(tc_no) < 0){
			strcpy(s.msg, "电文初始化失败。");
			Log::Trace("", __FUNCTION__, "电文初始化失败[{0}]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		if (epex.SetValue("bapiheader", "msgtype", 0, msg_type) < 0 ||
			epex.SetValue("bapiheader", "freeuse1", 0, fr1) < 0 ||
			epex.SetValue("bapiheader", "freeuse2", 0, fr2) < 0 ||
			epex.SetValue("bapiheader", "freeuse3", 0, fr3) < 0 ||
			epex.SetValue("bapiheader", "freeuse4", 0, fr4) < 0 ||
			epex.SetValue("bapiheader", "freeuse5", 0, fr5) < 0
			)
		{
			sprintf(s.msg, "设置电文数据失败bapiheader，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (epex.SetValue("input", "t_name", 0, in_name) < 0 ||
			epex.SetValue("input", "t_rout1", 0, in_rout1) < 0 ||
			epex.SetValue("input", "t_rout2", 0, in_rout2) < 0
			)
		{
			sprintf(s.msg, "设置电文数据失败input，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (epex.SetValue("output", "t_name", 0, out_name) < 0 ||
			epex.SetValue("output", "t_rout1", 0, out_rout1) < 0 ||
			epex.SetValue("output", "t_rout2", 0, out_rout2) < 0
			)
		{
			sprintf(s.msg, "设置电文数据失败output，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (epex.SetValue("zcho_qm_lot", "rueckmelnr", 0, tqmts2132["rueckmelnr"].ToString()) < 0 ||
			epex.SetValue("zcho_qm_lot", "prueflos", 0, tqmts2132["prueflos"].ToString()) < 0 ||
			epex.SetValue("zcho_qm_lot", "vornr", 0, tqmts2132["vornr"].ToString()) < 0 ||
			epex.SetValue("zcho_qm_lot", "vorktxt", 0, tqmts2103["pruefbemkt"].ToString()) < 0 ||
			epex.SetValue("zcho_qm_lot", "merknr", 0, tqmts2132["merknr"].ToString()) < 0 ||
			epex.SetValue("zcho_qm_lot", "erfassart", 0, tqmts2103["satzart"].ToString()) < 0 ||
			epex.SetValue("zcho_qm_lot", "werk", 0, tqmts2132["auswmgwrk"].ToString()) < 0 ||
			epex.SetValue("zcho_qm_lot", "art", 0, tqmts2131["satzart"].ToString()) < 0 ||
			epex.SetValue("zcho_qm_lot", "herkunft", 0, "") < 0 ||//检验批源
			epex.SetValue("zcho_qm_lot", "entstehdat", 0, tqmts2132["pruefdatuv"].ToString()) < 0 ||
			epex.SetValue("zcho_qm_lot", "ersteller", 0, "") < 0 ||//创建数据记录的用户名
			epex.SetValue("zcho_qm_lot", "matnr", 0, "") < 0 ||//物料号
			epex.SetValue("zcho_qm_lot", "ktextmat", 0, "") < 0 ||//物料短文本
			epex.SetValue("zcho_qm_lot", "charg", 0, "") < 0 ||//批号
			epex.SetValue("zcho_qm_lot", "lagortchrg", 0, "") < 0 ||//库存地点
			epex.SetValue("zcho_qm_lot", "subsys", 0, tqmts2132["subsys"].ToString()) < 0 ||
			epex.SetValue("zcho_qm_lot", "prplatz", 0, "") < 0 ||//工作中心
			epex.SetValue("zcho_qm_lot", "satzart", 0, tqmts2132["satzart"].ToString()) < 0 ||
			epex.SetValue("zcho_qm_lot", "kzrzwang", 0, tqmts2132["rueckmelnr"].ToString()) < 0 ||
			epex.SetValue("zcho_qm_lot", "statusv", 0, "") < 0 ||//说明记录状态
			epex.SetValue("zcho_qm_lot", "pmethode", 0, "") < 0 ||//检验方法
			epex.SetValue("zcho_qm_lot", "pmtkurztxt", 0, tqmts2131["pruefbemkt"].ToString()) < 0 ||
			epex.SetValue("zcho_qm_lot", "kurztext", 0, tqmts2132["pruefbemkt"].ToString()) < 0 ||
			epex.SetValue("zcho_qm_lot", "stellen", 0, 0) < 0 ||//小数位数
			epex.SetValue("zcho_qm_lot", "masseinhsw", 0, 0) < 0 ||//检验特性的计量单位
			epex.SetValue("zcho_qm_lot", "sollwert", 0, tqmts2132["stuecknr"].ToString()) < 0 ||
			epex.SetValue("zcho_qm_lot", "toleranzob", 0, "") < 0 ||//规范上限
			epex.SetValue("zcho_qm_lot", "toleranzun", 0, "") < 0 ||//规范下限
			epex.SetValue("zcho_qm_lot", "plausioben", 0, "") < 0 ||//实际上限
			epex.SetValue("zcho_qm_lot", "plausiunte", 0, "") < 0 ||//实际下限
			epex.SetValue("zcho_qm_lot", "katab1", 0, tqmts2132["katab"].ToString()) < 0 ||//目录条目是一个选择集
			epex.SetValue("zcho_qm_lot", "katalgart1", 0, tqmts2132["katalgart"].ToString()) < 0 ||
			epex.SetValue("zcho_qm_lot", "auswmgwrk1", 0, tqmts2132["auswmgwrk"].ToString()) < 0 ||
			epex.SetValue("zcho_qm_lot", "auswmenge1", 0, tqmts2132["auswmenge"].ToString()) < 0 ||
			epex.SetValue("zcho_qm_lot", "timestamp", 0, "") < 0 ||
			epex.SetValue("zcho_qm_lot", "mblnr", 0, "") < 0 ||//物料凭证编号
			epex.SetValue("zcho_qm_lot", "zeile", 0, 0) < 0 ||//物料凭证中的项目
			epex.SetValue("zcho_qm_lot", "verwmerkm", 0, "") < 0 ||//主文件检验特性
			epex.SetValue("zcho_qm_lot", "dummy10", 0, tqmts2132["param_row"].ToString()) < 0 ||//附加信息的文本行
			epex.SetValue("zcho_qm_lot", "dummy20", 0, "") < 0 ||//附加信息的文本行
			epex.SetValue("zcho_qm_lot", "dummy30", 0, "") < 0 ||//附加信息的文本行
			epex.SetValue("zcho_qm_lot", "kzrast", 0, "") < 0 ||//指示：检验点的采样过程
			epex.SetValue("zcho_qm_lot", "aufnr", 0, 0) < 0 ||//订单号
			epex.SetValue("zcho_qm_lot", "ktextlos", 0, tqmts2131["pruefbemkt"].ToString()) < 0
			)
		{
			sprintf(s.msg, "设置电文数据失败zcho_qm_lot，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (epex.SendTele() < 0)
		{
			sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		epex.Uninitialize();
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;

}
