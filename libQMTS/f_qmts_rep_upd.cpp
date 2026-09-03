/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   guxia
Version:    1.0
Date:     2015-07-11 09:13:56
Description: 代表成分选定函数：
//将代表成分写入TQMTS29和TQMTQQ0（合并钢样和气体样）
//判定结果更新到TQMTS23表
//如果代表成分合格，则TQMTS23表最终出钢记号=预定出钢记号;若否，清空TQMTS23表最终出钢记号
//若代表成分合格，调用物料函数修改炉次下每块板坯的最终出钢记号，发送代表成分电文到MMS
这个逻辑貌似没有了：如果炉次不合格，且如果已有板坯产出的话，把炉次对应的板坯封锁
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件，请包含在""中

 
BM2_FUNCTION_IMPORT
int f_qm200001_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //炉次成分电文
#if defined(_SYS_PES) || defined(_SYS_MES) 

//int f_cm_20qx03_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //代表样不合格的炉次发送炉次信息到智慧质量
#endif

int f_mmsm99(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//调用炼钢函数更新最终出钢记号

int f_mm0099(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//物料跨产线的入口主函数，不合格封锁
/////////////update by yiling 20160630 PES功能没有抛合同跟踪，部署到MES时，把MMS处置接收电文改造下，直接调用。

int f_qmtj_cm_x000qh_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//不合格则自动封锁,本函数跟电文无关，修改自电文接收函数而已

int f_qmtj_log(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);//写质量处置履历

#ifdef _QM_YC
int f_qmbs_yc_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//炉次异常写TQMTS23表
#endif
int f_qmts_rep_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	//APP_BEGIN()
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	CString sqlstr("");
	CString sqlstr1(""); 
	int doFlag = 0;
	int ret = 0;
	int mat_count = 0;
	int count_25 = 0;
	CString s_st_sample_no_on = "";
	CString s_st_sample_no_h = "";
	CString cur_st_sample_no = "";
	CString QMTJBlock_log = "QMTJLOG";
	CString v_qm00_qmqx = "";
	/* 实体类定义 */
	CModel tqmts23("TQMTS23");
	CModel tqmts25("TQMTS25");
	CModel tqmts29("TQMTS29");
	CModel tmmsm01("TMMSM01");
	CModel tqmtqq0("TQMTQQ0");
#ifdef _SYS_MES
	CModel tqmtqb0("TQMTQB0");
#endif

	CModel tqmtjlg("TQMTJLG");
	//调用电文函数
	EIClass bcls_rec_200001;
	EIClass bcls_ret_200001;
	bcls_rec_200001.Tables[0].Columns.Add(DT_STRING, "PONO");
	bcls_rec_200001.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_200001.Tables[0].Columns.Add(DT_STRING, "DECI_ST_NO");
	bcls_rec_200001.Tables[0].Rows.Add();

	//调用电文函数
	EIClass bcls_rec_20qx03;
	EIClass bcls_ret_20qx03;
	bcls_rec_20qx03.Tables[0].Columns.Add(DT_STRING, "PONO");
	bcls_rec_20qx03.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_20qx03.Tables[0].Columns.Add(DT_STRING, "ST_NO");
	bcls_rec_20qx03.Tables[0].Columns.Add(DT_STRING, "DU_FLAG");
	bcls_rec_20qx03.Tables[0].Rows.Add();

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_upd(conn);
	CDbCommand cmd_tep0002(conn);

	//调物料函数用	
	CString RecBlock = "MM0099";//约定的块名
	EIClass bcls_rec_mm;
	EIClass bcls_ret_mm;
	bcls_rec_mm.Tables[0].set_TableName(RecBlock);
	bcls_rec_mm.Tables[RecBlock].Columns.Add(DT_STRING, "MAT_KIND");		/*物料类型*/
	bcls_rec_mm.Tables[RecBlock].Columns.Add(DT_STRING, "EVENT_ID");		/*事件标识*/
	bcls_rec_mm.Tables[RecBlock].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");	/*事件产线类型*/
	bcls_rec_mm.Tables[RecBlock].Columns.Add(DT_STRING, "SYSTEM_ID");		/*系统标识*/
	bcls_rec_mm.Tables[RecBlock].Columns.Add(DT_STRING, "FUNC_ID");			/*功能标识*/
	bcls_rec_mm.Tables[RecBlock].Columns.Add(DT_STRING, "EVENT_DESC");		/*事件描述*/
	bcls_rec_mm.Tables[RecBlock].Columns.Add(DT_STRING, "DEAL_CODE");		/*事件描述*/
	bcls_rec_mm.Tables[RecBlock].Columns.Add(DT_STRING, "HOLD_CAUSE_CODE");		/*事件描述*/


	//调炼钢物料函数用	
	CString MMSMBlock = "MM0099";//约定的块名
	EIClass bcls_rec_sm;
	EIClass bcls_ret_sm;
	bcls_rec_sm.Tables[0].set_TableName(MMSMBlock);
	bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "MAT_KIND");		/*物料类型*/
	bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "EVENT_ID");		/*事件标识*/
	bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");/*事件产线类型*/
	bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "SYSTEM_ID");		/*系统标识*/
	bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "FUNC_ID");		/*功能标识*/
	bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "MAT_NO");			/*材料号*/
	bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "FIN_ST_NO");		/*最终出钢记号*/
	bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "ST_NO");			/*出钢记号*/
	bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "EVENT_DESC");		/*事件描述*/

	//写质量处置履历用
	EIClass bcls_rec_qmtj;
	EIClass bcls_ret_qmtj;
	bcls_rec_qmtj.Tables[0].set_TableName(QMTJBlock_log);
	try
	{
		//-----------------------------------------------------------
		/* 获得输入参数 */
		tqmts25["PONO"] = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();
		tqmts25["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		tqmts25["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString().Trim();
		cur_st_sample_no = tqmts25["ST_SAMPLE_NO"];

		Log::Trace("", __FUNCTION__, "PONO=[{0}]", tqmts25["PONO"].ToString());
		Log::Trace("", __FUNCTION__, "HEAT_NO=[{0}]", tqmts25["HEAT_NO"].ToString());
		Log::Trace("", __FUNCTION__, "1 ST_SAMPLE_NO= [{0}]", tqmts25["ST_SAMPLE_NO"].ToString());

		if (tqmts25["PONO"].ToString().TrimOrBlank() == " ")
		{
			strcpy(s.msg, _RES("QM00S0006264")/*制造命令号不能为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tqmts25["HEAT_NO"].ToString().TrimOrBlank() == " ")
		{
			strcpy(s.msg, _RES("QM00S0004334")/*熔炼号不允许为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tqmts25["ST_SAMPLE_NO"].ToString().TrimOrBlank() == " ")
		{
			strcpy(s.msg, _RES("QM00S0004260")/*请选择或输入试样号。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//-----------将代表成分写入TQMTS29和TQMTQQ0-------------------------------------
		//清除TQMTS29信息
		tqmts29["PONO"] = tqmts25["PONO"];
		tqmts29.Delete("PONO");
		count_25 = 0;
		//写入TQMTS29代表成分信息(钢样)
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT * "
				"  FROM TQMTS25 "
				"  WHERE PONO = @pono "
				"    AND ST_SAMPLE_NO = @st_sample_no ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("pono", tqmts25["PONO"].ToString());
		cmd_inq.Parameters.Set("st_sample_no", tqmts25["ST_SAMPLE_NO"].ToString());
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tqmts25);
			count_25++;
			//赋值
			tqmts29.CopyFrom(tqmts25);
			tqmts29["ELM_VALUE"] = tqmts25["ELM_ACT"];

			//信息初始化
			tqmts29["REC_CREATOR"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts29["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts29["REC_REVISOR"] = " ";
			tqmts29["REC_REVISE_TIME"] = " ";
			tqmts29["ARCHIVE_FLAG"] = " ";

			//新增数据
			Log::Trace("", "", "tqmts29插入");
			tqmts29.Delete("HEAT_NO, ELM_CODE");

			tqmts29.Insert();

			/*新增TQMTQQ0*/
			tqmtqq0["REC_CREATOR"] = s.userid;
			tqmtqq0["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmtqq0["REC_REVISOR"] = " ";
			tqmtqq0["REC_REVISE_TIME"] = " ";
			tqmtqq0["ARCHIVE_FLAG"] = " ";
			tqmtqq0["PONO"] = tqmts25["PONO"];
			tqmtqq0["HEAT_NO"] = tqmts25["HEAT_NO"];
			tqmtqq0["VENDOR_CODE"] = " ";
			tqmtqq0["VENDOR_NAME"] = " ";
			tqmtqq0["ST_NO"] = tqmts25["ST_NO"];
			tqmtqq0["SAMPLE_LOT_NO"] = " ";
			tqmtqq0["ELM_CODE"] = tqmts25["ELM_CODE"];
			tqmtqq0["ELM_NAME"] = tqmts25["ELM_NAME"];
			tqmtqq0["ELM_ACT"] = tqmts25["ELM_ACT"];
			tqmtqq0["ELM_UNIT"] = tqmts29["ELM_UNIT"];
			tqmtqq0["PCH_JUDGE_CODE"] = " ";

			tqmtqq0.Delete("HEAT_NO,ELM_CODE"); //条件字段项
			Log::Trace("", "", "tqmtqq0插入");
			tqmtqq0.Insert();
#ifdef _SYS_MES
			tqmtqb0.CopyFrom(tqmtqq0);
#endif
		}
		cmd_inq.Close();
		if (count_25 == 0)
		{
			strcpy(s.msg, "选择的炼钢试样号尚无钢样实绩，不可作为代表样。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
#ifdef _SYS_MES
		tqmtqb0.Delete("HEAT_NO");
		tqmtqb0.Insert();
#endif

		//查询连铸或者模铸的O/N样最大的试样号，写入TQMTS29代表成分信息，和TQMTQQ0
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT MAX(ST_SAMPLE_NO) "
				"   FROM TQMTS24 "
				"  WHERE  PONO = @pono "
				"  AND ST_SAMPLE_DIV ='4' AND WHOLE_BACKLOG_CODE IN ('C','I') AND GAS_TYPE_DIV='1'  ";/*4-气体样。1-ON样*/
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("pono", tqmts25["PONO"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			s_st_sample_no_on = cmd_inq.GetString(1);
		}
		cmd_inq.Close();

		if (s_st_sample_no_on.Trim() != "")
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT * "
					"  FROM TQMTS25 "
					" WHERE PONO = @pono "
					"   AND ST_SAMPLE_NO = @st_sample_no ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("pono", tqmts25["PONO"].ToString());
			cmd_inq.Parameters.Set("st_sample_no", s_st_sample_no_on);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(tqmts25);

				//赋值
				tqmts29.CopyFrom(tqmts25);
				tqmts29["ELM_VALUE"] = tqmts25["ELM_ACT"];

				//信息初始化
				tqmts29["REC_CREATOR"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts29["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts29["REC_REVISOR"] = " ";
				tqmts29["REC_REVISE_TIME"] = " ";
				tqmts29["ARCHIVE_FLAG"] = " ";

				//新增数据
				tqmts29.Delete("HEAT_NO, ELM_CODE");

				tqmts29.Insert();

				/*新增TQMTQQ0*/
				tqmtqq0["REC_CREATOR"] = s.userid;
				tqmtqq0["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmtqq0["REC_REVISOR"] = " ";
				tqmtqq0["REC_REVISE_TIME"] = " ";
				tqmtqq0["ARCHIVE_FLAG"] = " ";
				tqmtqq0["PONO"] = tqmts25["PONO"];
				tqmtqq0["HEAT_NO"] = tqmts25["HEAT_NO"];
				tqmtqq0["VENDOR_CODE"] = " ";
				tqmtqq0["VENDOR_NAME"] = " ";
				tqmtqq0["ST_NO"] = tqmts25["ST_NO"];
				tqmtqq0["SAMPLE_LOT_NO"] = " ";
				tqmtqq0["ELM_CODE"] = tqmts25["ELM_CODE"];
				tqmtqq0["ELM_NAME"] = tqmts25["ELM_NAME"];
				tqmtqq0["ELM_ACT"] = tqmts25["ELM_ACT"];
				tqmtqq0["ELM_UNIT"] = tqmts29["ELM_UNIT"];
				tqmtqq0["PCH_JUDGE_CODE"] = " ";

				tqmtqq0.Delete("HEAT_NO, ELM_CODE"); //条件字段项
				tqmtqq0.Insert();
			}
			cmd_inq.Close();
		}

		//查询连铸或者模铸的H样最大的试样号，写入TQMTS29代表成分信息，和TQMTQQ0
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT MAX(ST_SAMPLE_NO) "
				"   FROM TQMTS24 "
				"  WHERE PONO = @pono "
				"  AND ST_SAMPLE_DIV ='4' AND WHOLE_BACKLOG_CODE IN ('C','I') AND GAS_TYPE_DIV='2'  ";/*4-气体样。2-H样*/
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("pono", tqmts25["PONO"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			s_st_sample_no_h = cmd_inq.GetString(1);
		}
		cmd_inq.Close();
		Log::Trace("", "", "TQMTS24最大试样号=【{0}】", s_st_sample_no_h);
		if (s_st_sample_no_h.Trim() != "")
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT * "
					"  FROM TQMTS25 "
					" WHERE PONO = @pono "
					"   AND ST_SAMPLE_NO = @st_sample_no ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("pono", tqmts25["PONO"].ToString());
			cmd_inq.Parameters.Set("st_sample_no", s_st_sample_no_h);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(tqmts25);

				//赋值
				tqmts29.CopyFrom(tqmts25);
				tqmts29["ELM_VALUE"] = tqmts25["ELM_ACT"];

				//信息初始化
				tqmts29["REC_CREATOR"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts29["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts29["REC_REVISOR"] = " ";
				tqmts29["REC_REVISE_TIME"] = " ";
				tqmts29["ARCHIVE_FLAG"] = " ";

				//新增数据
				Log::Trace("", "", "tqmts29插入");
				tqmts29.Delete("HEAT_NO, ELM_CODE");
				tqmts29.Insert();

				/*新增TQMTQQ0*/
				tqmtqq0["REC_CREATOR"] = s.userid;
				tqmtqq0["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmtqq0["REC_REVISOR"] = " ";
				tqmtqq0["REC_REVISE_TIME"] = " ";
				tqmtqq0["ARCHIVE_FLAG"] = " ";
				tqmtqq0["PONO"] = tqmts25["PONO"];
				tqmtqq0["HEAT_NO"] = tqmts25["HEAT_NO"];
				tqmtqq0["VENDOR_CODE"] = " ";
				tqmtqq0["VENDOR_NAME"] = " ";
				tqmtqq0["ST_NO"] = tqmts25["ST_NO"];
				tqmtqq0["SAMPLE_LOT_NO"] = " ";
				tqmtqq0["ELM_CODE"] = tqmts25["ELM_CODE"];
				tqmtqq0["ELM_NAME"] = tqmts25["ELM_NAME"];
				tqmtqq0["ELM_ACT"] = tqmts25["ELM_ACT"];
				tqmtqq0["ELM_UNIT"] = tqmts29["ELM_UNIT"];
				tqmtqq0["PCH_JUDGE_CODE"] = " ";

				tqmtqq0.Delete("HEAT_NO, ELM_CODE"); //条件字段项
				Log::Trace("", "", "tqmtqq0插入");

				tqmtqq0.Insert();
			}
			cmd_inq.Close();
		}

		//原本下面一段归纳判定结果的逻辑有漏洞（标准要求的元素实绩如果有缺的情况会漏判）
		//改为直接从TQMTS24表里取判定结果 updated by zhp on 20171107
		Log::Trace("", __FUNCTION__, "cur_st_sample_no= [{0}]", cur_st_sample_no);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT JUDGE_CODE "
				"   FROM TQMTS24 "
				"  WHERE PONO = @pono "
				"  AND ST_SAMPLE_NO = @ST_SAMPLE_NO"
				;
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("pono", tqmts25["PONO"].ToString());
		cmd_inq.Parameters.Set("ST_SAMPLE_NO", cur_st_sample_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tqmts23["JUDGE_CODE"] = cmd_inq.GetString(1);
		}
		cmd_inq.Close();

		if (tqmts23["JUDGE_CODE"].ToString() == "1")
			tqmts23["JUDGE_REMARK"] = "合格";
		if (tqmts23["JUDGE_CODE"].ToString() == "2")
			tqmts23["JUDGE_REMARK"] = "不合格";
		if (tqmts23["JUDGE_CODE"].ToString() == "0")
			tqmts23["JUDGE_REMARK"] = "未判";

		////---------------归纳元素判定结果，将合格与否的判定结果写入TQMTS23-----------------------
		////查询TQMTS29的元素判定结果
		//switch (conn->DatabaseKind)
		//{
		//case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//case DB_KIND_MSSQL:				// MS SQL Server数据库
		//case DB_KIND_ORACLE:	        // Oracle 数据库
		//default:						// 所有数据库适用，通用SQL语句
		//	sqlstr = " SELECT COUNT(1) "
		//		"    FROM TQMTS29 "
		//		" WHERE PONO = @pono "
		//		" AND ELM_OK not in ( 8,9) ";
		//	break;
		//}
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Set("pono", tqmts25["PONO"].ToString());
		//cmd_inq.ExecuteReader();
		//if (cmd_inq.Read())
		//{
		//	int i_count_1 = cmd_inq.GetInt32(1);
		//	
		//	if (i_count_1 <= 0)//元素全部合格
		//	{
		//		tqmts23["JUDGE_CODE"] = "1";
		//		tqmts23["JUDGE_REMARK"] = "合格";
		//	}
		//	else//有不合格或者未判的元素项
		//	{
		//		//查询是否有不合格项目
		//		switch (conn->DatabaseKind)
		//		{
		//		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//		case DB_KIND_MSSQL:				// MS SQL Server数据库
		//		case DB_KIND_ORACLE:	        // Oracle 数据库
		//		default:						// 所有数据库适用，通用SQL语句
		//			sqlstr1 = " SELECT COUNT(1) "
		//				"    FROM TQMTS29 "
		//				" WHERE PONO = @pono "
		//				" AND ELM_OK not in ( 8,9,0)  ";
		//			break;
		//		}
		//		cmd_inq1.SetCommandText(sqlstr1);
		//		cmd_inq1.Parameters.Set("pono", tqmts25["PONO"].ToString());
		//		cmd_inq1.ExecuteReader();
		//		if (cmd_inq1.Read())
		//		{
		//			int i_count_2 = cmd_inq1.GetInt32(1);

		//			if (i_count_2 >= 0)//有不合格的项目
		//			{
		//				tqmts23["JUDGE_CODE"] = "2";
		//				tqmts23["JUDGE_REMARK"] = "不合格";
		//			}
		//			else
		//			{
		//				tqmts23["JUDGE_CODE"] = "0";
		//				tqmts23["JUDGE_REMARK"] = "未判";
		//			}
		//		}
		//	}
		//}
		//cmd_inq.Close();

		//初始化信息
		tqmts23["JUDGE_MAKER"] = s.userid;
		tqmts23["JUDGE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
		//将判定结果修改到TQMTS23；如合格，把此表的预定出钢记号作为最终出钢记号；如不合格，清空此表的最终出钢记号
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " UPDATE TQMTS23 "
				" SET JUDGE_CODE = @judge_code, "
				"	  JUDGE_MAKER = @judge_maker, "
				"	  JUDGE_TIME = @judge_time, "
				"	  JUDGE_REMARK = @judge_remark ";

			Log::Trace("", "", "合格否：tqmts23.JUDGE_CODE[{0}]", tqmts23["JUDGE_CODE"].ToString());

			if (tqmts23["JUDGE_CODE"].ToString() == "1")  //合格时，把预定出钢记号作为最终出钢记号
			{
				sqlstr = sqlstr + "	  ,FIN_ST_NO = ST_NO ";
			}
			else
			{
				sqlstr = sqlstr + "	  ,FIN_ST_NO = ' '  ";//不合格时，清空最终出钢记号
			}
			sqlstr = sqlstr + " WHERE HEAT_NO = @heat_no ";
				
			break;
		}
		Log::Trace("", "", "UPDATE语句：sqlstr[{0}]", sqlstr);
		cmd_upd.SetCommandText(sqlstr);
		cmd_upd.Parameters.Set("judge_code", tqmts23["JUDGE_CODE"].ToString());
		cmd_upd.Parameters.Set("judge_maker", tqmts23["JUDGE_MAKER"].ToString());
		cmd_upd.Parameters.Set("judge_time", tqmts23["JUDGE_TIME"].ToString());
		cmd_upd.Parameters.Set("judge_remark", tqmts23["JUDGE_REMARK"].ToString());
		cmd_upd.Parameters.Set("heat_no", tqmts25["HEAT_NO"].ToString());
		cmd_upd.ExecuteNonQuery();
		cmd_upd.Close();

		//炉次异常更新炉次质量表
#ifdef _QM_YC
		doFlag = f_qmbs_yc_jud(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
#endif
		//若代表成分合格，先调用物料函数修改炉次下每块板坯的最终出钢记号，再发送炉次确定电文到MMS
		mat_count = 0;
		if (tqmts23["JUDGE_CODE"].ToString() == "1")
		{
			////////////////////////成分合格，则调用炼钢物料函数update by yiling 
			tqmts23["HEAT_NO"] = tqmts25["HEAT_NO"];
			tqmts23.Query("HEAT_NO");
			Log::Trace("", "", "tqmts23.ST_NO[{0}]", tqmts23["ST_NO"].ToString());

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT * "
					"   FROM TMMSM01 "
					" WHERE HEAT_NO = @heat_no "
					"   and IF_TRANSFER != '1' "//交接坯标志，1表示交接坯，不自动置最终出钢记号，需人工判定
					;
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tqmts25["HEAT_NO"].ToString());
			cmd_inq.ExecuteReader();

			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(tmmsm01);

				tmmsm01.MergeTo(bcls_rec_sm.Tables[MMSMBlock], false);
				Log::Trace("", "", "11111MAT_NO[{0}]mat_count[{1}]", tmmsm01["MAT_NO"].ToString(), mat_count);
				bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["MAT_KIND"] = tmmsm01["MAT_KIND"];
				bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["EVENT_ID"] = "QM70";//修改板坯上的最终出钢记号
				bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["EVENT_LINE_TYPE"] = "00";
				bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["SYSTEM_ID"] = "QMTS";
				bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["FUNC_ID"] = "f_qmts_rep_upd";
				bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["MAT_NO"] = tmmsm01["MAT_NO"];
				bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["FIN_ST_NO"] = tqmts23["ST_NO"];//把预定出钢记号作为最终出钢记号
				bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["ST_NO"] = tqmts23["ST_NO"];
				bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["EVENT_DESC"] = "代表成分选定";
				mat_count++;
			}
			cmd_inq.Close();

			Log::Trace("", "", "准备调用f_mmsm99...");

			if (mat_count != 0)
			{
				Log::Trace("", "", "bcls_rec_smget_Count[{0}]mat_count[{1}]", bcls_rec_sm.Tables[MMSMBlock].Rows.get_Count(), mat_count);
				doFlag = f_mmsm99(&bcls_rec_sm, &bcls_ret_sm, conn);
				if (doFlag != 0)
				{
					strcat(s.msg, _RES("调用f_mmsm99--一事件号QM70出错。"));
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}


		}

#ifdef _SYS_PES
		//发送炉次确定电文到MMS：含代表成分以及最终出钢记号,update by yiling 成分不合格也应该发电文
		bcls_rec_200001.Tables[0].Rows[0]["PONO"] = tqmts25["PONO"];
		bcls_rec_200001.Tables[0].Rows[0]["HEAT_NO"] = tqmts25["HEAT_NO"];
		bcls_rec_200001.Tables[0].Rows[0]["DECI_ST_NO"] = tqmts23["ST_NO"].ToString().TrimOrBlank();

		doFlag = f_qm200001_snd(&bcls_rec_200001, &bcls_ret_200001, conn);
		if (doFlag != 0)
		{
			Log::Trace("", "", "f_qm200001_snd() msg = [{0}]", s.msg);
			throw CApplicationException(-1, s.msg, log.Location);
		}
#endif
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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
	//在函数退出前，统一Close()操作
	cmd_inq.Close();

	return doFlag;
}

