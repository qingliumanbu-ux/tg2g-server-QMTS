/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      guxia
Version:     1.0
Date:        2015-07-06
Description: 为交接坯赋予新的炉号（老炉号+A）和新的出钢记号
**************************************************/
/*******
0、校核新炉号在QMTS28画面上委托检验产生过新的成分实绩；
1、调物料函数修改物料相关信息；发送处置电文；
2、判定新炉成分按新钢种要求是否合格；
3、写29表和Q0表（源头是2C表）；
4、写B0表和23表；
5、如判定不合，自动封锁；
6、发代表成分电文（如PES系统）；
7、写炼钢质量履历表
***********/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中









int f_qmtjp_00(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//记录操作履历 
int f_mmsm99(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//调用炼钢函数更行最终出钢记号
int f_qm200002_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //出钢记号判定下达电文(MMS的物料模块接收)
int f_qm200001_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //炉次成分电文
//int f_cm_x000qh_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);//PES处置电文发送
int f_qmts_jud(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//判定
int f_mm0099(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//物料跨产线的入口主函数，不合格封锁


// service入口
BM2F_ENTERACE(qmts22_hn_jud)


int f_qmts22_hn_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString s_mat_no = " ";
    CString s_mat_no_2a = " ";   //////做了板坯成分的板坯号
	CString s_judge_st_no = " ";
	CString s_new_heat_no = " ";
	CString s_new_pono = " ";
	CString event_program = "qmts22_hn_jud";
	CString event_code = "100020";
	CString keyvalue_1 = "材料号";
	CString keyvalue_2 = "终判出钢记号";
	CString keyvalue_3 = "原最终出钢记号";
	CString proc_content = " ";
	CString s_max_char = " ";
	char v_char[1];

	CString sqlstr = "";
	CString sqlstr_ts2b = "";
	CString sqlstr_ts2c = "";

	EIClass bcls_rec_qmtjp;   //记录操作履历用
	EIClass bcls_ret_qmtjp;
	bcls_rec_qmtjp.Tables[0].Columns.Add(DT_STRING, "EVENT_PROGRAM");
	bcls_rec_qmtjp.Tables[0].Columns.Add(DT_STRING, "EVENT_CODE");
	bcls_rec_qmtjp.Tables[0].Columns.Add(DT_STRING, "KEYVALUE_1");
	bcls_rec_qmtjp.Tables[0].Columns.Add(DT_STRING, "KEYVALUE_2");
	bcls_rec_qmtjp.Tables[0].Columns.Add(DT_STRING, "KEYVALUE_3");
	bcls_rec_qmtjp.Tables[0].Columns.Add(DT_STRING, "PROC_CONTENT");
	bcls_rec_qmtjp.Tables[0].Rows.Add();

	//调用物料函数，置板坯决定出钢记号、对板坯做释放
	EIClass bcls_rec_mm;
	EIClass bcls_ret_mm;
	bcls_rec_mm.Tables.Add("MM0099");
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");//事件号
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");//板坯号
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "PONO");//制造命令号
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "HEAT_NO");//熔炼号
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");//功能标识
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");//系统标识
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "ST_NO");//当前出钢记号
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "DECI_ST_NO");//决定出钢记号
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "FIN_ST_NO");//最终出钢记号
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "JUDGE_MAKER");//判定责任者
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "JUDGE_TIME");//判定时刻
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "JUDGE_ST_NO");//判定出钢记号
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "HOLD_FLAG");//封锁标记(0: 释放 1：管理封锁 2：质量封锁； 3: 管理封锁+质量封锁)
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_DESC");//事件描述
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "REL_TIME");//释放时刻
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "REL_MAKER");//释放责任者
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "REL_REMARK");//释放注释
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");//事件产线类型
	bcls_rec_mm.Tables["MM0099"].Rows.Add();

	//调物料函数用	
	CString RecBlock = "MM0099";//约定的块名
	EIClass bcls_rec_mm99;
	EIClass bcls_ret_mm99;
	bcls_rec_mm99.Tables[0].set_TableName(RecBlock);
	bcls_rec_mm99.Tables[RecBlock].Columns.Add(DT_STRING, "MAT_KIND");		/*物料类型*/
	bcls_rec_mm99.Tables[RecBlock].Columns.Add(DT_STRING, "EVENT_ID");		/*事件标识*/
	bcls_rec_mm99.Tables[RecBlock].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");	/*事件产线类型*/
	bcls_rec_mm99.Tables[RecBlock].Columns.Add(DT_STRING, "SYSTEM_ID");		/*系统标识*/
	bcls_rec_mm99.Tables[RecBlock].Columns.Add(DT_STRING, "FUNC_ID");			/*功能标识*/
	bcls_rec_mm99.Tables[RecBlock].Columns.Add(DT_STRING, "EVENT_DESC");		/*事件描述*/

	EIClass bcls_rec_s;
	EIClass bcls_ret_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "MAT_KIND");
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "MAT_LINE_TYPE");//区分板坯是炼钢侧还是热轧侧还是棒线侧的
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "DEAL_CODE");
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "DEFECT_FLAG");
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "PCH_JUDGE_ABN");//改配自动综判合格时，放备注信息
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "SAMPLE_LOT_NO");//强制改配时，把试批号清掉
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "FINAL_CUT_SURFACE_CODE");
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "HOLD_CAUSE_CODE");//
	bcls_rec_s.Tables[0].Rows.Add();

	//调用物料电文函数，置板坯决定出钢记号、对板坯做释放
	EIClass bcls_rec_200002;
	EIClass bcls_ret_200002;
	bcls_rec_200002.Tables.Add("MMSMSM");

	EIClass bcls_rec_f;   //计算组合元素、判定用
	EIClass bcls_ret_f;
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "TABLE_NAME");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "ST_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "PONO");
	bcls_rec_f.Tables[0].Rows.Add();

	EIClass bcls_rec_f2c;
	EIClass bcls_ret_f2c;

	//调用电文函数
	EIClass bcls_rec_200001;
	EIClass bcls_ret_200001;
	bcls_rec_200001.Tables[0].Columns.Add(DT_STRING, "PONO");
	bcls_rec_200001.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_200001.Tables[0].Columns.Add(DT_STRING, "DECI_ST_NO");
	bcls_rec_200001.Tables[0].Rows.Add();

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tqmts29("TQMTS29");
	CModel tqmtqq0("TQMTQQ0");
	CModel tqmts2b("TQMTS2B");
	CModel tqmts2c("TQMTS2C");
	CModel tqmts23("TQMTS23");
	CModel tqmtqb0("TQMTQB0");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_ts2b(conn);
	CDbCommand cmd_inq_ts2c(conn);
	CDbCommand cmd_inq_ts29(conn);
	CDbCommand cmd_upd(conn);

	try
	{
		//对输入信息循环处理
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//取得单行传入信息
			tmmsm01.Reset();
			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			s_mat_no = tmmsm01["MAT_NO"].ToString().Trim();
			s_judge_st_no = tmmsm01["JUDGE_ST_NO"].ToString().Trim();
			

			Log::Trace("", "", "qmts22_hn_jud IN:---s_mat_no = [{0}]", s_mat_no);
			Log::Trace("", "", "qmts22_hn_jud IN:---s_judge_st_no = [{0}]", s_judge_st_no);
			Log::Trace("", "", "qmts22_hn_jud IN:---s_new_heat_no = [{0}]", s_new_heat_no);
			Log::Trace("", "", "qmts22_hn_jud IN:---s_new_pono = [{0}]", s_new_pono);

			//-----------合理性检查------------------
			if (s_mat_no == "")
			{
				sprintf(s.msg, _RES("QM00S0004293")/*判定板坯号不可为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (s_judge_st_no == "")
			{
				sprintf(s.msg, _RES("QM00S0004171")/*出钢记号不可为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (s_judge_st_no.Trim() == "YY000000")
			{
				sprintf(s.msg, _RES("QM00S0003905")/*最终出钢记号不可为YY待判。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (s_judge_st_no != "ZZ000000")//报废不校验出钢记号是否在工艺卡中
			{
				int i_tqmts0x_num = 0;
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
				cmd_inq.Parameters.Set("st_no", s_judge_st_no.Trim());
				i_tqmts0x_num = cmd_inq.ExecuteScalar().ToInt32();
				cmd_inq.Close();

				if (i_tqmts0x_num <= 0)
				{
					CFormattable arguments[] = { (const char*)s_judge_st_no }; // 定义参数列表的数组
					CMessageFormat::Format(s.msg, _RES("QM00S0004000")/*出钢记号[{0}]在工艺卡中不存在，不能操作。*/, arguments, 1); //格式化字符串
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			//获取库存信息
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT * "
					"   FROM TMMSM01 "
					"  WHERE MAT_NO = @mat_no ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_no", s_mat_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				cmd_inq.Fetch(tmmsm01);
			}
			else
			{
				CFormattable arguments[] = { (const char*)s_mat_no }; // 定义参数列表的数组
				CMessageFormat::Format(s.msg, _RES("QM00S0004294")/*板坯[{0}]在物料主档中已不存在，无法终判。*/, arguments, 1); //格式化字符串
				throw CApplicationException(-1, s.msg, log.Location);
			}
			cmd_inq.Close();
			if (tmmsm01["ORDER_NO"].ToString().TrimOrBlank() != " ")////update by yiling 20160506
			{
				CFormattable arguments[] = { (const char*)s_mat_no }; // 定义参数列表的数组
				CMessageFormat::Format(s.msg, "板坯[{0}]请先脱合同，再改钢种。", arguments, 1); //格式化字符串
				throw CApplicationException(-1, s.msg, log.Location);
			}
			////获取当前熔炼倒4位在A-Z中的最大值
			//switch (conn->DatabaseKind)
			//{
			//case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//case DB_KIND_MSSQL:				// MS SQL Server数据库
			//case DB_KIND_ORACLE:	        // Oracle 数据库
			//default:						// 所有数据库适用，通用SQL语句
			//	sqlstr = "	SELECT NVL(MAX(SUBSTR(HEAT_NO,4,1)),' ') "
			//		"	FROM TQMTS29  "
			//		"	WHERE HEAT_NO LIKE  SUBSTR(@tmmsm01["HEAT_NO"].ToString(),0,3)||'%'||SUBSTR(@tmmsm01["HEAT_NO"].ToString(),5,4) "
			//		"	AND SUBSTR(HEAT_NO,4,1) >= 'A'  AND SUBSTR(HEAT_NO,4,1) <= 'Z' ";
			//	break;
			//}
			//cmd_inq_ts29.SetCommandText(sqlstr);
			//cmd_inq_ts29.Parameters.Set("tmmsm01.HEAT_NO", tmmsm01["HEAT_NO"].ToString());
			//cmd_inq_ts29.ExecuteReader();
			//if (cmd_inq_ts29.Read())
			//{
			//	s_max_char = cmd_inq_ts29.GetString(1);
			//}
			//else
			//{
			//	strcpy(s.msg, "从数据表[TQMTS29]获取熔炼号[" + tmmsm01["HEAT_NO"].ToString() + "]信息失败！");
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			//cmd_inq_ts29.Close();
			//Log::Trace("", "", "s_max_char = [{0}]", s_max_char);
			//if (s_max_char == 72)//为I,跳过J
			//{
			//	s_max_char = "J";
			//}
			//else if(s_max_char == 78)//为N,跳过O
			//{
			//	s_max_char = "P";
			//}
			//else if (s_max_char >= 65 && s_max_char < 90)
			//{
			//	s_max_char = s_max_char[0] + 1;
			//}
			//else if (s_max_char == "Z")
			//{

			//}
			//else
			//{
			//	s_max_char = "A";
			//}

			//if (s_max_char.Trim() != "")
			//{
			//	s_new_heat_no = tmmsm01["HEAT_NO"].ToString().Substring(0, 3) + s_max_char + tmmsm01["HEAT_NO"].ToString().Substring(4, 4);
			//	s_new_pono = s_new_heat_no;
			//}
			//else
			//{
			//	strcpy(s.msg, "熔炼号自动转换失败！");
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT HEAT_NO, PONO ,SLAB_NO  FROM TQMTS2A WHERE HEAT_NO =(SELECT MAX(HEAT_NO)"
					"  FROM TQMTS2A "
					" WHERE heat_no like @heat_no||'%') ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tmmsm01["HEAT_NO"].ToString().Trim());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				s_new_heat_no = cmd_inq.GetString(1);
				s_new_pono = cmd_inq.GetString(2);
				s_mat_no_2a = cmd_inq.GetString(3);
			}
			else
			{
				strcpy(s.msg, "找不到熔炼号[" + tmmsm01["HEAT_NO"].ToString() + "]下板坯成分委托。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			cmd_inq.Close();
			Log::Trace("", "", "s_new_heat_no = [{0}]", s_new_heat_no);
			Log::Trace("", "", "s_new_pono = [{0}]", s_new_pono);
			Log::Trace("", "", "s_mat_no_2a = [{0}]", s_mat_no_2a);

			//校验新生成的熔炼号是否已存在
			tqmts29.Reset();
			tqmts29["HEAT_NO"] = s_new_heat_no;
			tqmts29["PONO"] = s_new_pono;
			if (tqmts29.QueryCount("HEAT_NO,PONO") > 0)
			{
				strcpy(s.msg, "已存在熔炼号[" + tqmts29["HEAT_NO"].ToString() + "]制造命令号[" + tqmts29["PONO"].ToString() + "]的炉次代表成分，不可新增");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//校验新生成的熔炼号是否已原来一样
			if (tmmsm01["PONO"].ToString().Trim() == s_new_pono)
			{
				strcpy(s.msg, "材料[" + s_mat_no + "]原制造命令号[" + tmmsm01["PONO"].ToString() + "]与自动转换后的制造命令号[" + s_new_pono + "]一致,不能修改");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (tmmsm01["HEAT_NO"].ToString().Trim() == s_new_heat_no)
			{
				strcpy(s.msg, "材料[" + s_mat_no + "]原熔炼号[" + tmmsm01["HEAT_NO"].ToString() + "]与自动转换后的熔炼号[" + s_new_heat_no + "]一致,不能修改");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/********************处理***************************************/
			//1。调用物料函数  置板坯判定出钢记号、判定责任者、判定时间 
			//   物料要将最终出钢记号修改同判定出钢记号
			CString s_judge_time = CDateTime::Now().ToString("yyyyMMddHHmmss");
			Log::Trace("", "", "s_judge_st_no = [{0}]", s_judge_st_no);

			bcls_rec_mm.Tables["MM0099"].Rows[0]["EVENT_ID"] = "QM72";					//事件号QM73
			bcls_rec_mm.Tables["MM0099"].Rows[0]["MAT_NO"] = s_mat_no;					//板坯号
			bcls_rec_mm.Tables["MM0099"].Rows[0]["PONO"] = s_new_pono;				//制造命令号
			bcls_rec_mm.Tables["MM0099"].Rows[0]["HEAT_NO"] = s_new_heat_no;					//板坯号
			bcls_rec_mm.Tables["MM0099"].Rows[0]["FUNC_ID"] = "qmts22_hn_jud";				//功能标识
			bcls_rec_mm.Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "SM";					//系统标识
			bcls_rec_mm.Tables["MM0099"].Rows[0]["DECI_ST_NO"] = tmmsm01["DECI_ST_NO"];	//决定出钢记号
			bcls_rec_mm.Tables["MM0099"].Rows[0]["FIN_ST_NO"] = s_judge_st_no;			//最终出钢记号
			bcls_rec_mm.Tables["MM0099"].Rows[0]["ST_NO"] = s_judge_st_no;			//最终出钢记号
			bcls_rec_mm.Tables["MM0099"].Rows[0]["JUDGE_MAKER"] = s.userid;				//判定者 
			bcls_rec_mm.Tables["MM0099"].Rows[0]["JUDGE_ST_NO"] = s_judge_st_no;			//判定出钢记号 
			bcls_rec_mm.Tables["MM0099"].Rows[0]["JUDGE_TIME"] = s_judge_time;				//判定时间
			bcls_rec_mm.Tables["MM0099"].Rows[0]["HOLD_FLAG"] = "0";					//封锁标记(0: 释放)
			bcls_rec_mm.Tables["MM0099"].Rows[0]["EVENT_DESC"] = "板坯改判";			//事件描述
			bcls_rec_mm.Tables["MM0099"].Rows[0]["REL_TIME"] = s_judge_time;				//释放时刻
			bcls_rec_mm.Tables["MM0099"].Rows[0]["REL_MAKER"] = s.userid;				//释放责任者
			bcls_rec_mm.Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";			//事件产线类型
			bcls_rec_mm.Tables["MM0099"].Rows[0]["HOLD_FLAG"] = "0";					//封锁标记(0: 释放)
			bcls_rec_mm.Tables["MM0099"].Rows[0]["REL_REMARK"] = "板坯改判";			//释放注释

			Log::Trace("", "", "***************************板坯改判***************************");
			doFlag = f_mmsm99(&bcls_rec_mm, &bcls_ret_mm, conn);
			if (doFlag != 0)
			{
				Log::Trace("", "", "f_mmsm99() msg = [{0}]", s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
#ifdef _SYS_PES
			////////////////同步MMS电文，  update by yiling 20160105////////////////////
			if (bcls_rec_s.Tables[0].Rows.get_Count() <= 0)
			{
				bcls_rec_s.Tables[0].Rows.Add();
			}
			Log::Trace("", "", "发送QM72改钢种电文");
			bcls_rec_s.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
			bcls_rec_s.Tables[0].Rows[0]["MAT_KIND"] = tmmsm01["MAT_KIND"];
			bcls_rec_s.Tables[0].Rows[0]["MAT_LINE_TYPE"] = tmmsm01["MAT_LINE_TYPE"];
			bcls_rec_s.Tables[0].Rows[0]["DEAL_CODE"] = "Z";
			bcls_rec_s.Tables[0].Rows[0]["HOLD_CAUSE_CODE"] = " ";

			//调用发送处置信息的函数
	/*		doFlag = f_cm_x000qh_snd(&bcls_rec_s, &bcls_ret_s, conn);

			if (doFlag != 0)
			{
				strcat(s.msg, _RES("调用发送处置信息的函数出错。"));
				throw CApplicationException(-1, s.msg, log.Location);
			}*/
			//////////////同步MMS电文，  update by yiling 20160105////////////////////
#endif
			/////////////update by yiling20160517先置所有的元素为不需判，原因是钢坯样收上来判定不合格，置了判定结果，现在改判新的出钢记号，标准少了，就不会判定，原判定结果还在。
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " UPDATE TQMTS2C" 
					"    SET ELM_OK = @elm_ok "
					"  WHERE  SLAB_NO=@s_mat_no "
					 ;
				break;
			}

			Log::Trace("", "", "UPD---------sqlstr[{0}]", sqlstr);

			cmd_upd.SetCommandText(sqlstr);
			cmd_upd.Parameters.Set("s_mat_no", s_mat_no_2a);
			cmd_upd.Parameters.Set("elm_ok", "9");
			cmd_upd.ExecuteNonQuery();
			cmd_upd.Close();
			///////判定移到写29表前，因为L4会根据成分判定结果对材料进行封锁update by yiling 20160420
			tqmts2b.Reset();
			tqmts2b["SLAB_NO"] = s_mat_no_2a;
			if (!tqmts2b.Query("SLAB_NO"))
			{
				Log::Trace("", "", "根据板坯号[" + tqmts2b["SLAB_NO"].ToString() + "]获取板坯成分主信息[TQMTS2B]失败！");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			bcls_rec_f.Tables[0].Rows[0]["TABLE_NAME"] = "TQMTS2C";
			bcls_rec_f.Tables[0].Rows[0]["ST_SAMPLE_NO"] = tqmts2b["ST_SAMPLE_NO"];
			bcls_rec_f.Tables[0].Rows[0]["ST_NO"] = s_judge_st_no;
			bcls_rec_f.Tables[0].Rows[0]["PONO"] = s_new_pono;
			///////多个材料同时改时，做了成分的材料号，才调用判定和发送成分电文
			if (s_mat_no.Trim() == s_mat_no_2a.Trim())
			{
				//启动判定
				doFlag = f_qmts_jud(&bcls_rec_f, &bcls_ret_f, conn);
			}

			if (doFlag != 0)
			{
				Log::Trace("", "", "f_qmts_jud() msg = [{0}]", s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			/*新增新熔炼号板坯成分--START*/
			//查询工序下的试样信息
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr_ts2b = "SELECT * "
					"  FROM TQMTS2C "
					" WHERE SLAB_NO = @s_mat_no ";
				break;
			}
			cmd_inq_ts2c.SetCommandText(sqlstr_ts2b);
			cmd_inq_ts2c.Parameters.Set("s_mat_no", s_mat_no_2a);
			cmd_inq_ts2c.ExecuteQuery(bcls_rec_f2c.Tables[0]);

			int count_2c = bcls_rec_f2c.Tables[0].Rows.get_Count();
			if (count_2c == 0)
			{
				strcpy(s.msg, "获取不到材料号[" + s_mat_no + "]的钢坯样化学成分");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//清除TQMTS29信息
			tqmts29["PONO"] = s_new_pono;
			tqmts29["HEAT_NO"] = s_new_heat_no;
			tqmts29.Delete("PONO");

			int elm_num = 0;
			//获取各元素值并插试样子表
			for (i = 0; i < bcls_rec_f2c.Tables[0].Rows.get_Count(); i++)
			{
				tqmts2c.MergeFrom(bcls_rec_f2c.Tables[0].Rows[i]);

				Log::Trace("", __FUNCTION__, "qmts22_hn_jud IN:---tqmts2c.ELM_CODE[{0}]", (const char*)tqmts2c["ELM_CODE"].ToString());
				Log::Trace("", __FUNCTION__, "qmts22_hn_jud IN:---tqmts2c.ELM_NAME[{0}]", (const char*)tqmts2c["ELM_NAME"].ToString());
				Log::Trace("", __FUNCTION__, "qmts22_hn_jud IN:-- -tqmts2c.ELM_VALUE[{0}]", tqmts2c["ELM_VALUE"].ToDecimal().ToDouble());

				elm_num++;
				tqmts29.CopyFrom(tqmts2c);
				tqmts29["PONO"] = s_new_pono;
				tqmts29["HEAT_NO"] = s_new_heat_no;
				tqmts29["ST_NO"] = s_judge_st_no;
				tqmts29.TrimOrBlank();

				//信息初始化
				tqmts29["REC_CREATOR"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts29["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts29["REC_REVISOR"] = " ";
				tqmts29["REC_REVISE_TIME"] = " ";
				tqmts29["ARCHIVE_FLAG"] = " ";

				//新增TQMTs29数据
				Log::Trace("", __FUNCTION__, "tqmts29.Insert()");
				tqmts29.Delete("PONO,ELM_CODE");
				tqmts29.Insert();

				/*新增TQMTQQ0数据*/
				tqmtqq0["REC_CREATOR"] = s.userid;
				tqmtqq0["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmtqq0["REC_REVISOR"] = " ";
				tqmtqq0["REC_REVISE_TIME"] = " ";
				tqmtqq0["ARCHIVE_FLAG"] = " ";
				tqmtqq0["PONO"] = s_new_pono;
				tqmtqq0["HEAT_NO"] = tqmts29["HEAT_NO"];
				tqmtqq0["VENDOR_CODE"] = " ";
				tqmtqq0["VENDOR_NAME"] = " ";
				tqmtqq0["ST_NO"] = tqmts29["ST_NO"];
				tqmtqq0["SAMPLE_LOT_NO"] = " ";
				tqmtqq0["ELM_CODE"] = tqmts29["ELM_CODE"];
				tqmtqq0["ELM_NAME"] = tqmts29["ELM_NAME"];
				tqmtqq0["ELM_ACT"] = tqmts29["ELM_VALUE"];
				tqmtqq0["ELM_UNIT"] = tqmts29["ELM_UNIT"];
				tqmtqq0["PCH_JUDGE_CODE"] = " ";

				tqmtqq0.Delete("PONO, ELM_CODE"); //条件字段项
				Log::Trace("", __FUNCTION__, "tqmtqq0.Insert()");
				tqmtqq0.Insert();
				tqmtqb0.CopyFrom(tqmtqq0);
			}
			if (elm_num < 1)
			{
				strcpy(s.msg, _RES("QM00S0004336")/*元素实绩不能全为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
#ifdef _SYS_MES
			tqmtqb0.Delete("PONO");
			tqmtqb0.Insert();
#endif
			/*新增新熔炼号板坯成分--END*/



			tqmts2b.Reset();
			tqmts2b["SLAB_NO"] = s_mat_no_2a;
			if (!tqmts2b.Query("SLAB_NO"))
			{
				Log::Trace("", "", "根据板坯号[" + tqmts2b["SLAB_NO"].ToString() + "]获取板坯成分主信息[TQMTS2B]失败！");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//新增tqmts23实绩_炉次质量信息表
			tqmts23.Reset();
			tqmts23.TrimOrBlank();
			tqmts23["HEAT_NO"] = s_new_heat_no;
			tqmts23["PONO"] = s_new_pono;
			tqmts23["FIN_ST_NO"] = s_judge_st_no;
			tqmts23["REC_CREATOR"] = s.userid;
			tqmts23["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts23["REC_REVISOR"] = " ";
			tqmts23["REC_REVISE_TIME"] = " ";
			tqmts23["ARCHIVE_FLAG"] = " ";
			tqmts23["JUDGE_CODE"] = tqmts2b["JUDGE_CODE"];
			tqmts23["JUDGE_MAKER"] = s.userid;
			tqmts23["JUDGE_REMARK"] = " ";
			tqmts23["JUDGE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts23.Delete("HEAT_NO");
			tqmts23.Insert();


			//判定不合格自动封锁
			if (tqmts2b["JUDGE_CODE"].ToString().Trim() == "2")
			{
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT * "
						"   FROM TMMSM01 "
						" WHERE MAT_NO = @s_mat_no ";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("s_mat_no", s_mat_no);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tmmsm01.Reset();
					cmd_inq.Fetch(tmmsm01);

					tmmsm01.MergeTo(bcls_rec_mm99.Tables[RecBlock], false);

					bcls_rec_mm99.Tables[RecBlock].Rows[0]["MAT_KIND"] = tmmsm01["MAT_KIND"];
					bcls_rec_mm99.Tables[RecBlock].Rows[0]["EVENT_ID"] = "QM17";//材料质量封锁
					bcls_rec_mm99.Tables[RecBlock].Rows[0]["EVENT_LINE_TYPE"] = "00";
					bcls_rec_mm99.Tables[RecBlock].Rows[0]["SYSTEM_ID"] = "QMTS";
					bcls_rec_mm99.Tables[RecBlock].Rows[0]["FUNC_ID"] = "qmts22_hn_jud";
					bcls_rec_mm99.Tables[RecBlock].Rows[0]["EVENT_DESC"] = "代表成分不合格";
					bcls_rec_mm99.Tables[RecBlock].Rows[0]["HOLD_CAUSE_CODE"] = "JN02";

					doFlag = f_mm0099(&bcls_rec_mm99, &bcls_ret_mm99, conn);
					if (doFlag != 0)
					{
						strcat(s.msg, _RES("调用物料处置函数f_mm0099出错。"));
						throw CApplicationException(-1, s.msg, log.Location);
					}
					////////////////同步MMS电文，  update by yiling 20160513////////////////////
#ifdef _SYS_PES
					if (bcls_rec_s.Tables[0].Rows.get_Count() <= 0)
					{
						bcls_rec_s.Tables[0].Rows.Add();
					}
					Log::Trace("", "", "发送QM17处置电文");
					bcls_rec_s.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
					bcls_rec_s.Tables[0].Rows[0]["MAT_KIND"] = tmmsm01["MAT_KIND"];
					bcls_rec_s.Tables[0].Rows[0]["MAT_LINE_TYPE"] = tmmsm01["MAT_LINE_TYPE"];
					bcls_rec_s.Tables[0].Rows[0]["DEAL_CODE"] = "2";
					bcls_rec_s.Tables[0].Rows[0]["HOLD_CAUSE_CODE"] = "AH01";

					//调用发送处置信息的函数
					//doFlag = f_cm_x000qh_snd(&bcls_rec_s, &bcls_ret_s, conn);

					//if (doFlag != 0)
					//{
					//	strcat(s.msg, _RES("调用发送处置信息的函数出错。"));
					//	throw CApplicationException(-1, s.msg, log.Location);
					//}
#endif
					//////////////同步MMS电文，  update by yiling 20160513////////////////////
				}
			}
#ifdef _SYS_PES
			/*炉次确定上传电文--START*/
			bcls_rec_200001.Tables[0].Rows[0]["PONO"] = tqmts29["PONO"];
			bcls_rec_200001.Tables[0].Rows[0]["HEAT_NO"] = tqmts29["HEAT_NO"];
			bcls_rec_200001.Tables[0].Rows[0]["DECI_ST_NO"] = tqmts29["ST_NO"].ToString().TrimOrBlank();
			if(s_mat_no.Trim() == s_mat_no_2a.Trim())
			{
				doFlag = f_qm200001_snd(&bcls_rec_200001, &bcls_ret_200001, conn);
				if (doFlag != 0)
				{
					Log::Trace("", "", "f_qm200001_snd() msg = [{0}]", s.msg);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
#endif
			/*炉次确定上传电文--END*/

			//2。记录操作履历
			keyvalue_1 = "材料号" + s_mat_no;
			//keyvalue_2 = "终判出钢记号[" + s_judge_st_no+"]制造命令号["+s_new_pono+"]熔炼号["+s_new_heat_no+"]";
			//keyvalue_3 = "原最终出钢记号[" + tmmsm01["FIN_ST_NO"].ToString() + "]原制造命令号[" + tmmsm01["PONO"].ToString() + "]熔炼号[" + tmmsm01["HEAT_NO"].ToString() + "]";
			keyvalue_2 = "终判出钢记号[" + s_judge_st_no+"]";
			keyvalue_3 = "原最终出钢记号[" + tmmsm01["FIN_ST_NO"].ToString() + "]" ;
			proc_content = proc_content + "";

			bcls_rec_qmtjp.Tables[0].Rows[0]["EVENT_PROGRAM"] = event_program;  //事件相关程序
			bcls_rec_qmtjp.Tables[0].Rows[0]["EVENT_CODE"] = event_code;  //事件代码
			bcls_rec_qmtjp.Tables[0].Rows[0]["KEYVALUE_1"] = keyvalue_1;  //关键字串1
			bcls_rec_qmtjp.Tables[0].Rows[0]["KEYVALUE_2"] = keyvalue_2;  //关键字串2
			bcls_rec_qmtjp.Tables[0].Rows[0]["KEYVALUE_3"] = keyvalue_3;  //关键字串3
			bcls_rec_qmtjp.Tables[0].Rows[0]["PROC_CONTENT"] = proc_content;  //处理内容

			//调用履历函数
			doFlag = f_qmtjp_00(&bcls_rec_qmtjp, &bcls_ret_qmtjp, conn);
			if (doFlag != 0)
			{
				Log::Trace("", "", "f_qmtjp_00() msg = [{0}]", s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}//for

		Log::Trace("", "", "板坯终判成功！");
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
