/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      guxia
Version:     1.0
Date:        2015-07-06
Description: 钢坯改出钢记号
**************************************************/
/************
1、调物料函数修改物料相关信息；
2、发送钢坯判定结果电文；
3、对于厚板板坯，最终出钢记号与预定出钢记号不同要调代预替换；
4、写炼钢质量履历表
*********/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



int f_qmtjp_00(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//记录操作履历 
int f_mmsm99(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//调用炼钢函数更行最终出钢记号
int f_qm200002_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //出钢记号判定下达电文(MMS的物料模块接收)
BM2_FUNCTION_IMPORT
int f_qmtqhp_dele_resv_chg_new(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection * conn);//根据命令板坯号进行代预替换
/////////////update by yiling 20160630 PES功能没有抛合同跟踪，部署到MES时，把MMS处置接收电文改造下，直接调用。
int f_qmtj_cm_x000qh_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//,本函数跟电文无关，修改自电文接收函数而已
BM2_FUNCTION_IMPORT
//int f_cm_x000qh_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);//PES处置电文发送
BM2_FUNCTION_IMPORT
int f_cm_00x0qh_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);//PES处置电文发送
// service入口
BM2F_ENTERACE(qmts22_jud)


int f_qmts22_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString s_mat_no = " ";
	CString s_judge_st_no = " ";
	CString event_program = "qmts22_jud";
	CString event_code = "100020";
	CString keyvalue_1 = "材料号";
	CString keyvalue_2 = "终判出钢记号";
	CString keyvalue_3 = "原最终出钢记号";
	CString proc_content = " ";

	CString sqlstr = "";

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

	//调用物料电文函数，置板坯决定出钢记号、对板坯做释放
	EIClass bcls_rec_200002;
	EIClass bcls_ret_200002;
	bcls_rec_200002.Tables.Add("MMSMSM");

	if (!bcls_rec->Tables.Contains("QMZSBlock"))
	{
		bcls_rec->Tables.Add("QMZSBlock");
	}
	if (!bcls_rec->Tables["QMZSBlock"].Columns.Contains("PONO_SLAB"))
	{
		bcls_rec->Tables["QMZSBlock"].Columns.Add(DT_STRING, "PONO_SLAB");
	}
	if (!bcls_rec->Tables["QMZSBlock"].Columns.Contains("PONO"))
	{
		bcls_rec->Tables["QMZSBlock"].Columns.Add(DT_STRING, "PONO");
	}

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
	bcls_rec_f.Tables[QMTSBlock].Rows.Add();

	EIClass bcls_rec_mm_h;	//调用物料函数，调用自动脱合同 update by yiling 20190102
	EIClass bcls_ret_mm_h;
	bcls_rec_mm_h.Tables[0].Columns.Add(DT_STRING, "MAT_KIND");		/*物料类型*/
	bcls_rec_mm_h.Tables[0].Columns.Add(DT_STRING, "EVENT_ID");		/*事件标识*/
	bcls_rec_mm_h.Tables[0].Columns.Add(DT_STRING, "EVENT_LINE_TYPE"); /*事件产线类型*/
	bcls_rec_mm_h.Tables[0].Columns.Add(DT_STRING, "SYSTEM_ID");		/*系统标识*/
	bcls_rec_mm_h.Tables[0].Columns.Add(DT_STRING, "FUNC_ID");			/*功能标识*/
	bcls_rec_mm_h.Tables[0].Columns.Add(DT_STRING, "MAT_NO");			/*材料号*/
	bcls_rec_mm_h.Tables[0].Columns.Add(DT_STRING, "EVENT_DESC");		/*事件描述*/
	bcls_rec_mm_h.Tables[0].Columns.Add(DT_STRING, "DEAL_CODE");		/*处置代码*/
	bcls_rec_mm_h.Tables[0].Rows.Add();
	//发送电文用
	EIClass bcls_rec_s;
	EIClass bcls_ret_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "MAT_KIND");
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "MAT_LINE_TYPE");//区分板坯是炼钢侧还是热轧侧还是棒线侧的
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "DEAL_CODE");
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "DEFECT_FLAG");
	bcls_rec_s.Tables[0].Rows.Add();
	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sm03_inq(conn);
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

			Log::Trace("", "", "qmts22_jud IN:---s_mat_no = [{0}]", s_mat_no);
			Log::Trace("", "", "qmts22_jud IN:---s_judge_st_no = [{0}]", s_judge_st_no);

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
			////update by yiling 20170209 
			if (tmmsm01["MAT_STATUS"].ToString().Trim() == "24")
			{
				CFormattable arguments[] = { (const char*)s_mat_no }; // 定义参数列表的数组
				CMessageFormat::Format(s.msg, "板坯[{0}]已编入计划，不能改判。", arguments, 1); //格式化字符串
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/********************处理***************************************/
			//1。调用物料函数  置板坯判定出钢记号、判定责任者、判定时间 
			//   物料要将最终出钢记号修改同判定出钢记号
			CString s_judge_time = CDateTime::Now().ToString("yyyyMMddHHmmss");
			Log::Trace("", "", "s_judge_st_no = [{0}]", s_judge_st_no);

			bcls_rec_mm.Tables["MM0099"].Rows[0]["EVENT_ID"] = "QM70";					//事件号QM73
			bcls_rec_mm.Tables["MM0099"].Rows[0]["MAT_NO"] = s_mat_no;					//板坯号
			bcls_rec_mm.Tables["MM0099"].Rows[0]["PONO"] = tmmsm01["PONO"];				//制造命令号
			bcls_rec_mm.Tables["MM0099"].Rows[0]["ST_NO"] = s_judge_st_no;				//制造命令号
			bcls_rec_mm.Tables["MM0099"].Rows[0]["FUNC_ID"] = "qmts22_jud";				//功能标识
			bcls_rec_mm.Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "QMTS";					//系统标识
			bcls_rec_mm.Tables["MM0099"].Rows[0]["DECI_ST_NO"] = tmmsm01["DECI_ST_NO"];	//决定出钢记号
			bcls_rec_mm.Tables["MM0099"].Rows[0]["FIN_ST_NO"] = s_judge_st_no;			//最终出钢记号
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
				Log::Trace("", "", "f_mmsm_qm73() msg = [{0}]", s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			Log::Trace("", "", "***************************板坯改判电文***************************");
			tmmsm96.Reset();
			tmmsm96.MergeFrom(bcls_rec_mm.Tables["MM0099"].Rows[0]);
			bcls_rec_200002.Tables["MMSMSM"].Clear();
			tmmsm96.MergeTo(bcls_rec_200002.Tables["MMSMSM"], false);
			Log::Trace("", "", "tmmsm01.PREC_ST_NO[{0}]s_judge_st_no[{1}]", tmmsm01["PREC_ST_NO"].ToString(), s_judge_st_no);
#ifdef _SYS_PES
			doFlag = f_qm200002_snd(&bcls_rec_200002,&bcls_ret_200002,conn);
			if(doFlag != 0)
			{    
				Log::Trace("", "","f_qm200002_snd() msg = [{0}]", s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (s_judge_st_no.TrimOrBlank() != tmmsm01["PREC_ST_NO"].ToString().TrimOrBlank() && tmmsm01["ORDER_NO"].ToString().TrimOrBlank() != " ")
			{
				bcls_rec_mm.Tables["MM0099"].Rows.Clear();
				tmmsm01.MergeTo(bcls_rec_mm.Tables["MM0099"], false);
				bcls_rec_mm.Tables["MM0099"].Rows[0]["MAT_KIND"] = tmmsm01["MAT_KIND"];
				bcls_rec_mm.Tables["MM0099"].Rows[0]["EVENT_ID"] = "QM16";//脱合同
				bcls_rec_mm.Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
				bcls_rec_mm.Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "QMTS";
				bcls_rec_mm.Tables["MM0099"].Rows[0]["FUNC_ID"] = "qmts22_jud";
				bcls_rec_mm.Tables["MM0099"].Rows[0]["EVENT_DESC"] = "板坯终判成分不合";
				bcls_rec_mm.Tables["MM0099"].Rows[0]["REMAIN_CAUSE_CODE"] = "QM01";//余材原因代码
				bcls_rec_mm.Tables["MM0099"].Rows[0]["REMAIN_REMARK"] = "质量脱合同";//余材注释

				doFlag = f_mmsm99(&bcls_rec_mm, &bcls_ret_mm, conn);
				if (doFlag != 0)
				{
					strcat(s.msg, _RES("调用物料处置函数f_mm0099出错。"));
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
#endif

			


			


#if defined(_SYS_MMS) || defined(_SYS_MES) 

			if(s_judge_st_no.TrimOrBlank()  != tmmsm01["PREC_ST_NO"].ToString().TrimOrBlank() )//最终出钢记号与预定出钢记号不同要调代预替换
			{
				if (bcls_rec->Tables["QMZSBlock"].Rows.get_Count() == 0)
				{
					bcls_rec->Tables["QMZSBlock"].Rows.Add();
				}
#ifdef _LINE_HP
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = " SELECT PONO_SLAB "
						"   FROM TMMSM03 "
						"  WHERE MAT_NO =  @MAT_NO"
						" AND PONO_SLAB !=' '"
						"  ORDER BY PONO_SLAB ASC ";
					break;
				}
				cmd_sm03_inq.SetCommandText(sqlstr);
				cmd_sm03_inq.Parameters.Set("MAT_NO", s_mat_no);
				cmd_sm03_inq.ExecuteReader();
				while (cmd_sm03_inq.Read())
				{

					tmmsm96["PONO_SLAB"] = cmd_sm03_inq.GetString(1).TrimOrBlank();
					Log::Trace("", "", "tmmsm96.PONO_SLAB[{0}]tmmsm01.PONO[{1}]", tmmsm96["PONO_SLAB"].ToString(), tmmsm01["PONO"].ToString());


					bcls_rec->Tables["QMZSBlock"].Rows[0]["PONO_SLAB"] = tmmsm96["PONO_SLAB"];
					bcls_rec->Tables["QMZSBlock"].Rows[0]["PONO"] = tmmsm96["PONO"];

					doFlag = f_qmtqhp_dele_resv_chg_new(bcls_rec, bcls_ret, conn);

					if (doFlag != 0)
					{
						s.flag = doFlag;
						throw CApplicationException(-1, s.msg, s.svc_name);
					}


				}
				cmd_sm03_inq.Close();
#endif
				Log::Trace("", "", "tmmsm01.ORDER_NO[{0}]tmmsm01.MAT_NO[{1}]", tmmsm01["ORDER_NO"].ToString(),tmmsm01["MAT_NO"].ToString());
				//////合同不为空时，自动脱合同 update by yiling 20190102
				if(tmmsm01["ORDER_NO"].ToString() != " ")
				{
					Log::Trace("", "", "合同不为空时tmmsm01.ORDER_NO[{0}]tmmsm01.MAT_NO[{1}]", tmmsm01["ORDER_NO"].ToString(),tmmsm01["MAT_NO"].ToString());
					bcls_rec_mm_h.Tables[0].Rows.Clear();
					tmmsm01.MergeTo(bcls_rec_mm_h.Tables[0],false);
					bcls_rec_mm_h.Tables[0].Rows[0]["MAT_NO"] = s_mat_no;
					bcls_rec_mm_h.Tables[0].Rows[0]["MAT_KIND"] = tmmsm01["MAT_KIND"];
					bcls_rec_mm_h.Tables[0].Rows[0]["EVENT_ID"] = "QM16";//脱合同
					bcls_rec_mm_h.Tables[0].Rows[0]["EVENT_LINE_TYPE"] = "00";
					bcls_rec_mm_h.Tables[0].Rows[0]["SYSTEM_ID"] = "QMTS";
					bcls_rec_mm_h.Tables[0].Rows[0]["FUNC_ID"] = "qmts22_jud";
					bcls_rec_mm_h.Tables[0].Rows[0]["EVENT_DESC"] = "板坯钢种终判改了最终内部钢种";
					bcls_rec_mm_h.Tables[0].Rows[0]["REMAIN_CAUSE_CODE"] = "0270";//余材原因代码
					bcls_rec_mm_h.Tables[0].Rows[0]["REMAIN_REMARK"] = "质量管理脱合同";//余材注释
					bcls_rec_mm_h.Tables[0].Rows[0]["DEAL_CODE"] = "H";//材料脱合同

					///////加处置电文，脱合同同步MMS，update by yiling 20150818
#if defined(_SYS_MMS) || defined(_SYS_MES)
					bcls_rec_s.Tables[0].Rows.Clear();
					tmmsm01.MergeTo(bcls_rec_s.Tables[0], false);
					bcls_rec_s.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
					bcls_rec_s.Tables[0].Rows[0]["MAT_KIND"] = tmmsm01["MAT_KIND"];
					bcls_rec_s.Tables[0].Rows[0]["MAT_LINE_TYPE"] = tmmsm01["MAT_LINE_TYPE"];
					bcls_rec_s.Tables[0].Rows[0]["DEAL_CODE"] = "H";
					bcls_rec_s.Tables[0].Rows[0]["DEFECT_FLAG"] = " ";
					////调用改造的函数进行脱合同
					doFlag = f_qmtj_cm_x000qh_rcv(&bcls_rec_mm_h, &bcls_ret_mm_h, conn);
					if (doFlag != 0)
					{
						strcat(s.msg, _RES("调用物料处置函数f_mm0099出错。"));
						throw CApplicationException(-1, s.msg, log.Location);
					}
#endif

				}
			}
#endif
			//这里不再需要发送脱合同的处置电文，因为物料模块会有同步到MMS/PES的操作，发了反而重复了  
			//zhp 20200225  根据八钢测试情况改的：电文200002在接收的时候已经做了脱合同的操作
//			if (tmmsm01["ORDER_NO"].ToString() != " ")
//			{
//				bcls_rec_s.Tables[0].Rows.Clear();
//				tmmsm01.MergeTo(bcls_rec_s.Tables[0], false);
//				bcls_rec_s.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
//				bcls_rec_s.Tables[0].Rows[0]["MAT_KIND"] = tmmsm01["MAT_KIND"];
//				bcls_rec_s.Tables[0].Rows[0]["MAT_LINE_TYPE"] = tmmsm01["MAT_LINE_TYPE"];
//				bcls_rec_s.Tables[0].Rows[0]["DEAL_CODE"] = "H";
//				bcls_rec_s.Tables[0].Rows[0]["DEFECT_FLAG"] = " ";
//#ifdef _SYS_PES
//				//调用发送处置信息的函数
//				doFlag = f_cm_x000qh_snd(&bcls_rec_s, &bcls_ret_s, conn);
//
//				if (doFlag != 0)
//				{
//					strcat(s.msg, _RES("调用发送处置信息的函数出错。"));
//					throw CApplicationException(-1, s.msg, log.Location);
//				}
//#endif
//#ifdef _SYS_MMS
//				//调用发送处置信息的函数
//				doFlag = f_cm_00x0qh_snd(&bcls_rec_s, &bcls_ret_s, conn);
//
//				if (doFlag != 0)
//				{
//					strcat(s.msg, _RES("调用发送处置信息的函数出错。"));
//					throw CApplicationException(-1, s.msg, log.Location);
//				}
//#endif
//			}
			
	//不用释放，因为脱合同时物料同时会释放 20200225 update by ZHP  根据八钢测试效果而改
			//Log::Trace("", "", "tmmsm01.HOLD_FLAG[{0}]tmmsm01.FINISH_FLAG[{1}]", tmmsm01["HOLD_FLAG"].ToString(), tmmsm01["FINISH_FLAG"].ToString());
			//if (tmmsm01["HOLD_FLAG"].ToString().TrimOrBlank() == "2"&&tmmsm01["FINISH_FLAG"].ToString().TrimOrBlank() != "1")
			//{

			//	bcls_rec_f.Tables[QMTSBlock].Rows[0]["MAT_KIND"] = tmmsm01["MAT_KIND"];
			//	bcls_rec_f.Tables[QMTSBlock].Rows[0]["EVENT_ID"] = "QM18";//材料质量释放
			//	bcls_rec_f.Tables[QMTSBlock].Rows[0]["EVENT_LINE_TYPE"] = "00";
			//	bcls_rec_f.Tables[QMTSBlock].Rows[0]["SYSTEM_ID"] = "QMTS";
			//	bcls_rec_f.Tables[QMTSBlock].Rows[0]["FUNC_ID"] = "qmts22_jud";
			//	bcls_rec_f.Tables[QMTSBlock].Rows[0]["MAT_NO"] = s_mat_no;
			//	bcls_rec_f.Tables[QMTSBlock].Rows[0]["EVENT_DESC"] = "板坯终判自动释放";
			//	bcls_rec_f.Tables[QMTSBlock].Rows[0]["REL_REMARK"] = "板坯终判自动释放";
			//	doFlag = f_mmsm99(&bcls_rec_f, &bcls_ret_f, conn);
			//	if (doFlag != 0)
			//	{
			//		Log::Trace("", "", "f_mmsm99() msg = [{0}]", s.msg);
			//		throw CApplicationException(-1, s.msg, log.Location);
			//	}
			//}

			//2。记录操作履历
			keyvalue_1 = "材料号" + s_mat_no;
			keyvalue_2 = "终判出钢记号" + s_judge_st_no;
			keyvalue_3 = "原最终出钢记号" + tmmsm01["FIN_ST_NO"].ToString();
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
