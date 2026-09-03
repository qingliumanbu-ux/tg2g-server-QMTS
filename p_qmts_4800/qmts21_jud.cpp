/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      guxia
Version:     1.0
Date:        2015-07-01
Description: 修改炉次最终出钢记号
//判定炉次成分，判定结果写TQMTS23表
//画面输入的炉次最终出钢记号，写入TQMTS23表
//修改炉次对应的板坯的最终出钢记号
//写履历
**************************************************/


//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件，请包含在""中




/*  外部函数申明  */
int f_qmts_jud(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//判定
int f_qmts_jud_01(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//推算出钢记号
int f_qmtjp_00(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//记录操作履历 
int f_mm0099(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn); 
/////////////update by yiling 20160630 PES功能没有抛合同跟踪，部署到MES时，把MMS处置接收电文改造下，直接调用。
int f_qmtj_cm_x000qh_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//,本函数跟电文无关，修改自电文接收函数而已
BM2_FUNCTION_IMPORT
int f_mmsm99(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//调用炼钢函数更行最终出钢记号
BM2_FUNCTION_IMPORT
//int f_cm_x000qh_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);//PES处置电文发送

// service入口
BM2F_ENTERACE(qmts21_jud)


int f_qmts21_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	int mat_count = 0;
	CDecimal mat_act_wt = 0;

	CString s_heat_no = " ";
	CString s_fin_st_no = " ";

	CString event_program = "qmts21_jud";
	CString event_code = "100010";  //炉次改判
	CString keyvalue_1 = "熔炼号";
	CString keyvalue_2 = "终判出钢记号";
	CString keyvalue_3 = " ";
	CString proc_content = " ";

	CString sqlstr = "";
	CString sqlstr1 = "";

	//计算组合元素、判定用
	EIClass bcls_rec_jud;
	EIClass bcls_ret_jud;
	bcls_rec_jud.Tables[0].Columns.Add(DT_STRING, "TABLE_NAME");
	bcls_rec_jud.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_jud.Tables[0].Columns.Add(DT_STRING, "ST_NO");
	bcls_rec_jud.Tables[0].Columns.Add(DT_STRING, "PONO");
	bcls_rec_jud.Tables[0].Rows.Add();

	//修改板坯最终出钢记号用
	EIClass bcls_rec_jud_01;
	EIClass bcls_ret_jud_01;
	bcls_rec_jud_01.Tables[0].Columns.Add(DT_STRING, "PONO");
	bcls_rec_jud_01.Tables[0].Columns.Add(DT_STRING, "ST_NO");
	bcls_rec_jud_01.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_jud_01.Tables[0].Rows.Add();

	//记录操作履历用
	EIClass bcls_rec_qmtjp;
	EIClass bcls_ret_qmtjp;
	bcls_rec_qmtjp.Tables[0].Columns.Add(DT_STRING, "EVENT_PROGRAM");
	bcls_rec_qmtjp.Tables[0].Columns.Add(DT_STRING, "EVENT_CODE");
	bcls_rec_qmtjp.Tables[0].Columns.Add(DT_STRING, "KEYVALUE_1");
	bcls_rec_qmtjp.Tables[0].Columns.Add(DT_STRING, "KEYVALUE_2");
	bcls_rec_qmtjp.Tables[0].Columns.Add(DT_STRING, "KEYVALUE_3");
	bcls_rec_qmtjp.Tables[0].Columns.Add(DT_STRING, "PROC_CONTENT");
	bcls_rec_qmtjp.Tables[0].Columns.Add(DT_STRING, "MAT_ACT_WT");
	bcls_rec_qmtjp.Tables[0].Rows.Add();
	//调炼钢物料函数用	
	CString MMSMBlock = "MM0099";//约定的块名
	EIClass bcls_rec_sm;
	EIClass bcls_ret_sm;
	bcls_rec_sm.Tables[0].set_TableName(MMSMBlock);
	bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "MAT_KIND");		/*物料类型*/
	bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "EVENT_ID");		/*事件标识*/
	bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");	/*事件产线类型*/
	bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "SYSTEM_ID");		/*系统标识*/
	bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "FUNC_ID");			/*功能标识*/
	bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "MAT_NO");			/*功能标识*/
	bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "FIN_ST_NO");			/*功能标识*/
	bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "ST_NO");			/*功能标识*/
	bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "EVENT_DESC");

	//发送电文用
	EIClass bcls_rec_s;
	EIClass bcls_ret_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "MAT_KIND");
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "MAT_LINE_TYPE");//区分板坯是炼钢侧还是热轧侧还是棒线侧的
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "DEAL_CODE");
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "DEFECT_FLAG");
	bcls_rec_s.Tables[0].Rows.Add();

	EIClass bcls_rec_f;	//调用物料函数，改最终出钢记号后自动释放
	EIClass bcls_ret_f;
	CString QMTSBlock = "MM0099";
	if (!bcls_rec_f.Tables.Contains("MM0099"))
	{
		bcls_rec_f.Tables.Add(QMTSBlock);
	}
	bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "MAT_KIND");		/*物料类型*/
	bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "EVENT_ID");		/*事件标识*/
	bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "EVENT_LINE_TYPE"); /*事件产线类型*/
	bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "SYSTEM_ID");		/*系统标识*/
	bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "FUNC_ID");			/*功能标识*/
	bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "MAT_NO");			/*材料号*/
	bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "FIN_ST_NO");		/*最终出钢记号*/
	bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "ST_NO");			/*出钢记号*/
	bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "EVENT_DESC");		/*事件描述*/
	bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "REL_REMARK");		/*释放注释*/
	bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "DEFECT_CLASS");		/*缺陷等级*/
	bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "DEFECT_CODE");		/*缺陷代码*/

	/* 实体类定义 */
	CModel tqmts23("TQMTS23");
	CModel tqmts29("TQMTS29");
	CModel tmmsm01("TMMSM01");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_upd(conn);

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
	bcls_rec_mm.Tables[RecBlock].Columns.Add(DT_STRING, "REMAIN_CAUSE_CODE");  /*余材原因代码*/
	bcls_rec_mm.Tables[RecBlock].Columns.Add(DT_STRING, "REMAIN_REMARK");      /*余材注释*/
	bcls_rec_mm.Tables[MMSMBlock].Columns.Add(DT_STRING, "DEAL_CODE");

	try
	{
		//获得输入参数
		s_heat_no = bcls_rec->Tables[0].Rows[0]["heat_no"].ToString().Trim();
		s_fin_st_no = bcls_rec->Tables[0].Rows[0]["fin_st_no"].ToString().Trim();

		Log::Trace("", "", "qmts21_jud IN:---s_heat_no = [{0}]", s_heat_no);
		Log::Trace("", "", "qmts21_jud IN:---s_fin_st_no = [{0}]", s_fin_st_no);

		//-----------合理性检查------------------
		if (s_heat_no.Trim() == "")
		{
			strcpy(s.msg, _RES("QM00S0004334")/*熔炼号不允许为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (s_fin_st_no.Trim() == "")
		{
			sprintf(s.msg, _RES("QM00S0004171")/*出钢记号不可为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		CDecimal i_tqmts0x_num = 0;
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT COUNT(1) "
				"   FROM TQMTS0X "
				"  WHERE ST_NO = @st_no ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no", s_fin_st_no.Trim());
		i_tqmts0x_num = cmd_inq.ExecuteScalar();
		cmd_inq.Close();
		if (i_tqmts0x_num <= 0)
		{
			CFormattable arguments[] = { s_fin_st_no }; // 定义参数列表的数组
			CMessageFormat::Format(s.msg, _RES("QM00S0004000")/*出钢记号[{0}]在工艺卡中不存在，不能操作。*/, arguments, 1); //格式化字符串
			throw CApplicationException(-1, s.msg, log.Location);
		}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"   FROM TQMTS23 "
				"  WHERE HEAT_NO = @heat_no ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", s_heat_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			cmd_inq.Fetch(tqmts23);
		}
		cmd_inq.Close();

		if ( tqmts23["JUDGE_CODE"].ToString().Trim() == "1")
		{
			CFormattable arguments[] = { (const char*)tqmts23["PONO"].ToString() }; // 定义参数列表的数组
			CMessageFormat::Format(s.msg, _RES("QM00S0004290")/*制造命令号[{0}]已经做过终判，不能再终判。*/, arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tqmts23["JUDGE_CODE"].ToString().Trim() == "0")
		{
			CFormattable arguments[] = { (const char*)tqmts23["PONO"].ToString() }; // 定义参数列表的数组
			CMessageFormat::Format(s.msg, _RES("QM00S0006261")/*制造命令号[{0}]的代表成分尚未选定，不能终判。*/, arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}


		//------------------处理-------------------------------
		//炉次成分判定
		//启动判定
		bcls_rec_jud.Tables[0].Rows[0]["TABLE_NAME"] = "TQMTS29";
		bcls_rec_jud.Tables[0].Rows[0]["HEAT_NO"] = tqmts23["HEAT_NO"];
		bcls_rec_jud.Tables[0].Rows[0]["ST_NO"] = s_fin_st_no;
		bcls_rec_jud.Tables[0].Rows[0]["PONO"] = tqmts23["PONO"];

		doFlag = f_qmts_jud(&bcls_rec_jud, &bcls_ret_jud, conn);

		if (doFlag != 0)
		{
			Log::Trace("", "", "f_qmts_jud() msg = [{0}]", s.msg);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tqmts23["JUDGE_CODE"] = bcls_ret_jud.Tables[0].Rows[0]["JUDGE_CODE"].ToString().TrimOrBlank();
		Log::Trace("", "", "f_qmts_jud() tqmts23.JUDGE_CODE = [{0}]", tqmts23["JUDGE_CODE"].ToString());
		if (tqmts23["JUDGE_CODE"].ToString() == "1")
		{
			tqmts23["JUDGE_REMARK"] = "合格";
			tqmts23["FIN_ST_NO"] = tqmts23["ST_NO"];
		}
		else if (tqmts23["JUDGE_CODE"].ToString() == "2")
		{
			tqmts23["JUDGE_REMARK"] = "不合格";
		}
		else
		{
			tqmts23["JUDGE_CODE"] = "0";
			tqmts23["JUDGE_REMARK"] = "未判";
		}
		//---------------将判定结果写入TQMTS23-----------------------
		////查询TQMTS29的元素判定结果
		//switch (conn->DatabaseKind)
		//{
		//case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//case DB_KIND_MSSQL:				// MS SQL Server数据库
		//case DB_KIND_ORACLE:	        // Oracle 数据库
		//default:						// 所有数据库适用，通用SQL语句
		//	sqlstr = " SELECT count(1) "
		//		"   FROM TQMTS29 "
		//		" WHERE PONO = @pono "
		//		" AND ELM_OK not in ( 8,9) ";
		//	break;
		//}
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Set("pono", tqmts23["PONO"].ToString());
		//cmd_inq.ExecuteReader();
		//if (cmd_inq.Read())
		//{
		//	int i_count_1 = cmd_inq.GetInt32(1);

		//	if (i_count_1 <= 0)//元素全部合格
		//	{
		//		tqmts23["JUDGE_CODE"] = "1";
		//		tqmts23["JUDGE_REMARK"] = "合格";
		//		tqmts23["FIN_ST_NO"] = tqmts23["ST_NO"];
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
		//				" AND ELM_OK <> 0 ";
		//			break;
		//		}
		//		cmd_inq1.SetCommandText(sqlstr1);
		//		cmd_inq1.Parameters.Set("pono", tqmts23["PONO"].ToString());
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

		//将判定结果修改到TQMTS23
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " UPDATE TQMTS23 "
				" SET JUDGE_CODE = @judge_code, "
				" JUDGE_MAKER = @judge_maker, "
				" JUDGE_TIME = @judge_time, "
				" JUDGE_REMARK = @judge_remark, "
				" FIN_ST_NO = @fin_st_no "
				" WHERE HEAT_NO = @heat_no ";
			break;
		}
		cmd_upd.SetCommandText(sqlstr);
		cmd_upd.Parameters.Set("judge_code", tqmts23["JUDGE_CODE"].ToString());
		cmd_upd.Parameters.Set("judge_maker", tqmts23["JUDGE_MAKER"].ToString());
		cmd_upd.Parameters.Set("judge_time", tqmts23["JUDGE_TIME"].ToString());
		cmd_upd.Parameters.Set("judge_remark", tqmts23["JUDGE_REMARK"].ToString());
		cmd_upd.Parameters.Set("fin_st_no", s_fin_st_no.TrimOrBlank());
		cmd_upd.Parameters.Set("heat_no", s_heat_no);
		cmd_upd.ExecuteNonQuery();
		cmd_upd.Close();

		//---------------修改板坯最终出钢记号---------------------
		bcls_rec_jud_01.Tables[0].Rows[0]["PONO"] = tqmts23["PONO"];	//制造命令号
		bcls_rec_jud_01.Tables[0].Rows[0]["ST_NO"] = s_fin_st_no;		//最终出钢记号
		bcls_rec_jud_01.Tables[0].Rows[0]["HEAT_NO"] = s_heat_no;		//熔炼号

		//调用修改板坯最终出钢记号函数
		doFlag = f_qmts_jud_01(&bcls_rec_jud_01, &bcls_ret_jud_01, conn);
		if (doFlag != 0)
		{
			Log::Trace("", "", "f_qmts_jud_01() msg = [{0}]", s.msg);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//修改炉次成分表最终出钢记号
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " UPDATE TQMTS29 "
				"    SET ST_NO = @st_no "
				"  WHERE HEAT_NO = @heat_no ";
			break;
		}
		cmd_upd.SetCommandText(sqlstr);
		cmd_upd.Parameters.Set("st_no", s_fin_st_no);
		cmd_upd.Parameters.Set("heat_no", s_heat_no);
		cmd_upd.ExecuteNonQuery();
		cmd_upd.Close();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " UPDATE TQMTS24 "
				"    SET ST_NO = @st_no, "
				"        FIN_ST_NO = @st_no "
				"  WHERE HEAT_NO = @heat_no ";
			break;
		}
		cmd_upd.SetCommandText(sqlstr);
		cmd_upd.Parameters.Set("st_no", s_fin_st_no);
		cmd_upd.Parameters.Set("heat_no", s_heat_no);
		cmd_upd.ExecuteNonQuery();
		cmd_upd.Close();

		//算炉次总重量，供后续统计分析用（驾驶舱算炼成率）
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT SUM(SLAB_WT) "
				"   FROM TMMSM33 "
				"  WHERE HEAT_NO = @heat_no ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", s_heat_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			mat_act_wt = cmd_inq.GetDecimal(1);
		}
		cmd_inq.Close();
		Log::Trace("", "", "mat_act_wt = [{0}]", mat_act_wt);

		//--------------记录操作履历---------------------
		keyvalue_1 = "熔炼号" + s_heat_no;
		keyvalue_2 = s_fin_st_no;//改后出钢记号
		keyvalue_3 = tqmts23["ST_NO"];//原出钢记号
		proc_content = proc_content + "";

		bcls_rec_qmtjp.Tables[0].Rows[0]["EVENT_PROGRAM"] = event_program;  //事件相关程序
		bcls_rec_qmtjp.Tables[0].Rows[0]["EVENT_CODE"] = event_code;  //事件代码
		bcls_rec_qmtjp.Tables[0].Rows[0]["KEYVALUE_1"] = keyvalue_1;  //关键字串1
		bcls_rec_qmtjp.Tables[0].Rows[0]["KEYVALUE_2"] = keyvalue_2;  //关键字串2
		bcls_rec_qmtjp.Tables[0].Rows[0]["KEYVALUE_3"] = keyvalue_3;  //关键字串3
		bcls_rec_qmtjp.Tables[0].Rows[0]["PROC_CONTENT"] = proc_content;  //处理内容
		bcls_rec_qmtjp.Tables[0].Rows[0]["MAT_ACT_WT"] = mat_act_wt;//炉次重量

		//调用履历函数
		doFlag = f_qmtjp_00(&bcls_rec_qmtjp, &bcls_ret_qmtjp, conn);
		if (doFlag != 0)
		{
			Log::Trace("", "", "f_qmtjp_00() msg = [{0}]", s.msg);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//if (tqmts23["JUDGE_CODE"].ToString() == "2" &&s_fin_st_no.Trim() != tqmts23["ST_NO"].ToString().Trim())
		if (s_fin_st_no.Trim() != tqmts23["ST_NO"].ToString().Trim())
		{
			//------------只有最终出钢记号与原出钢记号不同才能修改最终出钢记号，此时需要板坯--------------------
			mat_count = 0;

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
					" AND ORDER_NO != ' '";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", s_heat_no);
			cmd_inq.ExecuteReader();
			bcls_rec_mm.Tables[RecBlock].Rows.Clear();
			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(tmmsm01);

				tmmsm01.MergeTo(bcls_rec_mm.Tables[RecBlock], false);
				bcls_rec_mm.Tables[RecBlock].Rows[mat_count]["MAT_KIND"] = tmmsm01["MAT_KIND"];
				bcls_rec_mm.Tables[RecBlock].Rows[mat_count]["EVENT_ID"] = "QM16";//脱合同
				bcls_rec_mm.Tables[RecBlock].Rows[mat_count]["EVENT_LINE_TYPE"] = "00";
				bcls_rec_mm.Tables[RecBlock].Rows[mat_count]["SYSTEM_ID"] = "QMTS";
				bcls_rec_mm.Tables[RecBlock].Rows[mat_count]["FUNC_ID"] = "qmts21_jud";
				bcls_rec_mm.Tables[RecBlock].Rows[mat_count]["EVENT_DESC"] = "炉次终判改了最终内部钢种";
				bcls_rec_mm.Tables[RecBlock].Rows[mat_count]["REMAIN_CAUSE_CODE"] = "0270";//余材原因代码
				bcls_rec_mm.Tables[RecBlock].Rows[mat_count]["REMAIN_REMARK"] = "质量管理脱合同";//余材注释
				bcls_rec_mm.Tables[RecBlock].Rows[mat_count]["DEAL_CODE"] = "H";//材料脱合同

				///////加处置电文，脱合同同步MMS，update by yiling 20150818
				tmmsm01.MergeTo(bcls_rec_s.Tables[0], false);
				bcls_rec_s.Tables[0].Rows[mat_count]["MAT_NO"] = tmmsm01["MAT_NO"];
				bcls_rec_s.Tables[0].Rows[mat_count]["MAT_KIND"] = tmmsm01["MAT_KIND"];
				bcls_rec_s.Tables[0].Rows[mat_count]["MAT_LINE_TYPE"] = tmmsm01["MAT_LINE_TYPE"];
				bcls_rec_s.Tables[0].Rows[mat_count]["DEAL_CODE"] = "H";
				bcls_rec_s.Tables[0].Rows[mat_count]["DEFECT_FLAG"] = " ";
				
				mat_count++;

			}
			cmd_inq.Close();

			Log::Trace("", "", "准备调用物料处置函数...");
			bcls_rec_mm.prt();
			Log::Trace("", "", "tqmts23.ST_NO = [{0}]s_fin_st_no[{1}]mat_count[{2}]", tqmts23["ST_NO"].ToString(), s_fin_st_no, mat_count);

			if (mat_count != 0)
			{
#if defined(_SYS_MMS) || defined(_SYS_MES)
				//出钢记号与预定不一致时自动脱合同，本函数跟电文无关，修改自电文接收函数而已
				doFlag = f_qmtj_cm_x000qh_rcv(&bcls_rec_mm, &bcls_ret_mm, conn);
#endif
#ifdef _SYS_PES
				doFlag = f_mm0099(&bcls_rec_mm, &bcls_ret_mm, conn);
#endif		
			}
			if (doFlag != 0)
			{
				strcat(s.msg, _RES("调用物料处置函数f_mm0099出错。"));
				throw CApplicationException(-1, s.msg, log.Location);
			}			

			//这里不再需要发送脱合同的处置电文，因为物料模块会有同步到MMS/PES的操作，发了反而重复了  
			//zhp 20200225  根据八钢测试情况改的：电文200002在接收的时候已经做了脱合同的操作
//#ifdef _SYS_PES 
//			//调用发送处置信息的函数
//			Log::Trace("", "", "发送脱合同处置电文");
//			doFlag = f_cm_x000qh_snd(&bcls_rec_s, &bcls_ret_s, conn);
//
//			if (doFlag != 0)
//			{
//				strcat(s.msg, _RES("调用发送处置信息的函数出错。"));
//				throw CApplicationException(-1, s.msg, log.Location);
//			}
//#endif
		}

		//不用释放，因为脱合同时物料同时会释放 20200225 update by ZHP  根据八钢测试效果而改
		//炉次下的每个板坯，调物料跟踪函数
		//switch (conn->DatabaseKind)
		//{
		//case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//case DB_KIND_MSSQL:	        // MS SQL Server数据库
		//case DB_KIND_ORACLE:	        // Oracle 数据库
		//default: // 所有数据库适用，通用SQL语句
		//	sqlstr = "   SELECT * "
		//		"  FROM TMMSM01 "
		//		" WHERE HEAT_NO = @heat_no "
		//		"   AND hold_flag = '2' "
		//		"   AND finish_flag != '1' "
		//		" ORDER BY mat_no ASC ";
		//	break;
		//}
		//cmd_inq1.SetCommandText(sqlstr);
		//cmd_inq1.Parameters.Set("heat_no", s_heat_no);
		//cmd_inq1.ExecuteReader();
		//fetchRowCount = 0;
		//while (cmd_inq1.Read())
		//{
		//	cmd_inq1.Fetch(tmmsm01);

		//	Log::Trace("", "", "******************当前板坯号[{0}]", (const char*)tmmsm01["MAT_NO"].ToString());


		//	//*********************************   5. 调用物料函数*********************************//

		//	tmmsm01.MergeTo(bcls_rec_f.Tables[QMTSBlock], false);

		//	bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["MAT_KIND"] = tmmsm01["MAT_KIND"];
		//	bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["EVENT_ID"] = "QM18";//材料质量释放
		//	bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["EVENT_LINE_TYPE"] = "00";
		//	bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["SYSTEM_ID"] = "QMTS";
		//	bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["FUNC_ID"] = "qmts21_jud";
		//	bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["MAT_NO"] = tmmsm01["MAT_NO"];
		//	bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["EVENT_DESC"] = "炉次终判自动释放";
		//	bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["REL_REMARK"] = "板坯终判自动释放";
		//	fetchRowCount++;
		//}
		//cmd_inq1.Close();

		//Log::Trace("", "", "当前炉次涉及的板坯块数[{0}]，非0时调物料函数", fetchRowCount);

		////调用物料函数
		//i = bcls_rec_f.Tables[QMTSBlock].Rows.get_Count();
		//if (i > 0)
		//{
		//	doFlag = f_mmsm99(&bcls_rec_f, &bcls_ret_f, conn);
		//	if (doFlag != 0)
		//	{
		//		Log::Trace("", "", "f_mmsm99() msg = [{0}]", s.msg);
		//		throw CApplicationException(-1, s.msg, log.Location);
		//	}
		//}

		Log::Trace("", "", "炉次终判成功！");
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

	return doFlag;
}
