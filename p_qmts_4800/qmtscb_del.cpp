/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-04-18
Description: 带溜坯处置处置关闭
**************************************************/
//框架头文件
#include "stdafx.h"
BM2_FUNCTION_IMPORT
//外部函数声明
int f_mmsm_t80r91_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
//业务头文件

BM2F_ENTERACE(qmtscb_del)

int f_qmtscb_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	

	/* 业务变量 */
	CModel tqmtscb00("TQMTSCB00_DR");
	CModel tqmtscb01("TQMTSCB01_DR");
	CModel tqmtscb02("TQMTSCB02_DR");
	CModel tqmtscb03("TQMTSCB03_DR");
	CModel tqmtscb04("TQMTSCB04_DR");
	CModel tqmtscb05("TQMTSCB05_DR");
	CModel tqmtscb06("TQMTSCB06_DR");
	CModel tqmtscb07("TQMTSCB07_DR");
	CModel tqmtscb08("TQMTSCB08_DR");
	CModel tqmtscb09("TQMTSCB09_DR");
	CModel tqmtscb10("TQMTSCB03_FX");

	CModel hqmtscb00("HQMTSCB00_DR");
	CModel hqmtscb01("HQMTSCB01_DR");
	CModel hqmtscb02("HQMTSCB02_DR");
	CModel hqmtscb03("HQMTSCB03_DR");
	CModel hqmtscb04("HQMTSCB04_DR");
	CModel hqmtscb05("HQMTSCB05_DR");
	CModel hqmtscb06("HQMTSCB06_DR");
	CModel hqmtscb07("HQMTSCB07_DR");
	CModel hqmtscb08("HQMTSCB08_DR");
	CModel hqmtscb09("HQMTSCB09_DR");
	CModel hqmtscb10("HQMTSCB03_FX");
	CModel ttk0001("TTK0001");
	CModel tqmtscb1a("TQMTSCB11A_DR");
	CModel tqmtscb1b("TQMTSCB11B_DR");
	CModel tqmtscb1c("TQMTSCB11C");
	CModel tqmtscb1d("TQMTSCB11D_DR");
	CModel tmmsm0r91("TMMSM0R91");
	CModel tmmsmgy05("TMMSMGY05");
	CModel tqmtscbw5("TMMSMW5");
	CModel tmmsm2a_yl2("TMMSM2A_YL2");
	CModel tmmsmopr = CModel("TMMSMOPR");//操作记录表
	
	CString v_operate = "";
	CString v_back2 = "";
	CDecimal v_steel_wt = 0;
	CDecimal v_steel_wt_new = 0;
	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inqu(conn);
	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("PRO_DIV"))
		{
			v_operate = bcls_rec->Tables[0].Rows[0]["PRO_DIV"].ToString().Trim();
			Log::Trace("", "", "条件查询v_operate[{0}],", v_operate);
		}
		if (v_operate == "00")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{

				tqmtscb00["KM_CODE"] = bcls_rec->Tables[0].Rows[i]["KM_CODE"].ToString().TrimOrBlank();
				tqmtscb00["MAT_CODE"] = bcls_rec->Tables[0].Rows[i]["MAT_CODE"].ToString().TrimOrBlank();
				tqmtscb00["DATE_C"] = bcls_rec->Tables[0].Rows[i]["DATE_C"].ToString().TrimOrBlank();
				tqmtscb00["PRICE_TYPE"] = " ";
				if (tqmtscb00.QueryCount("KM_CODE,MAT_CODE,DATE_C,PRICE_TYPE")>0)
				{
					hqmtscb00.CopyFrom(tqmtscb00);
					hqmtscb00["REC_CREATOR"] = s.userid;
					hqmtscb00["REC_CREATE_TIME"] = datetime;
					hqmtscb00.Insert();
				}
				tqmtscb00.Delete("KM_CODE,MAT_CODE,DATE_C,PRICE_TYPE");
			}
		}
		
		
		if (v_operate == "0A")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{

				tqmtscb00["KM_CODE"] = bcls_rec->Tables[0].Rows[i]["KM_CODE"].ToString().TrimOrBlank();
				tqmtscb00["MAT_CODE"] = bcls_rec->Tables[0].Rows[i]["MAT_CODE"].ToString().TrimOrBlank();
				tqmtscb00["DATE_C"] = bcls_rec->Tables[0].Rows[i]["DATE_C"].ToString().TrimOrBlank();
				tqmtscb00["PRICE_TYPE"] = "BZ";
				if (tqmtscb00.QueryCount("KM_CODE,MAT_CODE,DATE_C,PRICE_TYPE")>0)
				{
					hqmtscb00.CopyFrom(tqmtscb00);
					hqmtscb00["REC_CREATOR"] = s.userid;
					hqmtscb00["REC_CREATE_TIME"] = datetime;
					hqmtscb00.Insert();
				}
				tqmtscb00.Delete("KM_CODE,MAT_CODE,DATE_C,PRICE_TYPE");
			}
		}
		if (v_operate == "01")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{

				tqmtscb01["AREA_CODE"] = bcls_rec->Tables[0].Rows[i]["AREA_CODE"].ToString().TrimOrBlank();
				tqmtscb01["MAT_CODE"] = bcls_rec->Tables[0].Rows[i]["MAT_CODE"].ToString().TrimOrBlank();
				tqmtscb01["DATE_C"] = bcls_rec->Tables[0].Rows[i]["DATE_C"].ToString().TrimOrBlank();

				if (tqmtscb01.QueryCount("DATE_C,AREA_CODE,MAT_CODE")>0)
				{
					hqmtscb01.CopyFrom(tqmtscb01);
					hqmtscb01["REC_CREATOR"] = s.userid;
					hqmtscb01["REC_CREATE_TIME"] = datetime;
					hqmtscb01.Insert();
				}
				tqmtscb01.Delete("DATE_C,AREA_CODE,MAT_CODE");
			}
		}
		if (v_operate == "02")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{

				tqmtscb02["MAT_CODE_DR"] = bcls_rec->Tables[0].Rows[i]["MAT_CODE_DR"].ToString().TrimOrBlank();
				tqmtscb02["DATE_C"] = bcls_rec->Tables[0].Rows[i]["DATE_C"].ToString().TrimOrBlank();

				if (tqmtscb02.QueryCount("DATE_C,MAT_CODE_DR")>0)
				{
					hqmtscb02.CopyFrom(tqmtscb02);
					hqmtscb02["REC_CREATOR"] = s.userid;
					hqmtscb02["REC_CREATE_TIME"] = datetime;
					hqmtscb02.Insert();
				}
				tqmtscb02.Delete("DATE_C,MAT_CODE_DR");
			}
		}
		if (v_operate == "03")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				tqmtscb03["MAT_CODE_DR"] = bcls_rec->Tables[0].Rows[i]["MAT_CODE_DR"].ToString().TrimOrBlank();
				tqmtscb03["DATE_C"] = bcls_rec->Tables[0].Rows[i]["DATE_C"].ToString().TrimOrBlank();

				if (tqmtscb03.QueryCount("DATE_C,MAT_CODE_DR")>0)
				{
					hqmtscb03.CopyFrom(tqmtscb03);
					hqmtscb03["REC_CREATOR"] = s.userid;
					hqmtscb03["REC_CREATE_TIME"] = datetime;
					hqmtscb03.Insert();
				}
				tqmtscb03.Delete("DATE_C,MAT_CODE_DR");
			}
		}
		if (v_operate == "04")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{

				tqmtscb04["MAT_CODE_DR"] = bcls_rec->Tables[0].Rows[i]["MAT_CODE_DR"].ToString().TrimOrBlank();
				

				if (tqmtscb04.QueryCount("MAT_CODE_DR")>0)
				{
					hqmtscb04.CopyFrom(tqmtscb04);
					hqmtscb04["REC_CREATOR"] = s.userid;
					hqmtscb04["REC_CREATE_TIME"] = datetime;
					hqmtscb04.Insert();
				}
				tqmtscb04.Delete("MAT_CODE_DR");
			}
		}
		if (v_operate == "05")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{

				tqmtscb05["PRICE_DATE"] = bcls_rec->Tables[0].Rows[i]["PRICE_DATE"].ToString().TrimOrBlank();
				tqmtscb05["ST_NO"] = bcls_rec->Tables[0].Rows[i]["ST_NO"].ToString().TrimOrBlank();
				
				if (tqmtscb05.QueryCount("PRICE_DATE,ST_NO")>0)
				{
					hqmtscb05.CopyFrom(tqmtscb05);
					hqmtscb05["REC_CREATOR"] = s.userid;
					hqmtscb05["REC_CREATE_TIME"] = datetime;
					hqmtscb05.Insert();
				}
				tqmtscb05.Delete("PRICE_DATE,ST_NO");
			}
		}
		if (v_operate == "06")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{

				tqmtscb06["MAT_CODE"] = bcls_rec->Tables[0].Rows[i]["MAT_CODE"].ToString().TrimOrBlank();
				tqmtscb06["DATE_C"] = bcls_rec->Tables[0].Rows[i]["DATE_C"].ToString().TrimOrBlank();

				if (tqmtscb06.QueryCount("MAT_CODE,DATE_C")>0)
				{
					hqmtscb06.CopyFrom(tqmtscb06);
					hqmtscb06["REC_CREATOR"] = s.userid;
					hqmtscb06["REC_CREATE_TIME"] = datetime;
					hqmtscb06.Insert();
				}
				tqmtscb06.Delete("MAT_CODE,DATE_C");
			}
		}
		if (v_operate == "07")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{

				tqmtscb07["MAT_CODE"] = bcls_rec->Tables[0].Rows[i]["MAT_CODE"].ToString().TrimOrBlank();
				tqmtscb07["I_YEAR"] = bcls_rec->Tables[0].Rows[i]["I_YEAR"].ToString().TrimOrBlank();
				
				if (tqmtscb07.QueryCount("MAT_CODE,I_YEAR")>0)
				{
					hqmtscb07.CopyFrom(tqmtscb07);
					hqmtscb07["REC_CREATOR"] = s.userid;
					hqmtscb07["REC_CREATE_TIME"] = datetime;
					hqmtscb07.Insert();
				}
				tqmtscb07.Delete("MAT_CODE,I_YEAR");
			}
		}
		if (v_operate == "08")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{

				tqmtscb08["MAT_CODE_DR"] = bcls_rec->Tables[0].Rows[i]["MAT_CODE_DR"].ToString().TrimOrBlank();

				if (tqmtscb08.QueryCount("MAT_CODE_DR")>0)
				{
					hqmtscb08.CopyFrom(tqmtscb08);
					hqmtscb08["REC_CREATOR"] = s.userid;
					hqmtscb08["REC_CREATE_TIME"] = datetime;
					hqmtscb08.Insert();
				}
				tqmtscb08.Delete("MAT_CODE_DR");
			}
		}
		if (v_operate == "09")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{

				tqmtscb09["STEEL_GRADE"] = bcls_rec->Tables[0].Rows[i]["STEEL_GRADE"].ToString().TrimOrBlank();
				
				if (tqmtscb09.QueryCount("STEEL_GRADE")>0)
				{
					hqmtscb09.CopyFrom(tqmtscb09);
					hqmtscb09["REC_CREATOR"] = s.userid;
					hqmtscb09["REC_CREATE_TIME"] = datetime;
					hqmtscb09.Insert();
				}
				tqmtscb09.Delete("STEEL_GRADE");
			}
		}
		if (v_operate == "10")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{

				tqmtscb10["GRADE_TYPE1"] = bcls_rec->Tables[0].Rows[i]["GRADE_TYPE1"].ToString().TrimOrBlank();
				tqmtscb10["PROCESS_ROUTE"] = bcls_rec->Tables[0].Rows[i]["PROCESS_ROUTE"].ToString().TrimOrBlank();
				tqmtscb10["ZB_DESC"] = bcls_rec->Tables[0].Rows[i]["ZB_DESC"].ToString().TrimOrBlank();

				if (tqmtscb10.QueryCount("GRADE_TYPE1,PROCESS_ROUTE,ZB_DESC")>0)
				{
					hqmtscb10.CopyFrom(tqmtscb10);
					hqmtscb10["REC_CREATOR"] = s.userid;
					hqmtscb10["REC_CREATE_TIME"] = datetime;
					hqmtscb10.Insert();
				}
				tqmtscb10.Delete("GRADE_TYPE1,PROCESS_ROUTE,ZB_DESC");
			}
		}
		if (v_operate == "11")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{

				ttk0001.Reset();
				ttk0001.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				ttk0001.Delete("MAT_CODE,MAT_CODE_T");
			}
		}
		if (v_operate == "1A")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb1a.Reset();
				tqmtscb1a.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				tqmtscb1a.Delete("YEAR,YEAR_MON,ST_NO,KM_CODE");

			}
		}
		if (v_operate == "W5")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscbw5.Reset();
				tqmtscbw5.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				tqmtscbw5.Delete("MAT_CODE,ELM_CODE");

			}
		}
		if (v_operate == "DW")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tmmsm2a_yl2.Reset();
				tmmsm2a_yl2.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				tmmsm2a_yl2.Delete("MAT_CODE");

			}
		}
		if (v_operate == "1B")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb1b.Reset();
				tqmtscb1b.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				tqmtscb1b.Delete("ST_NO");
				
			}
		}
		if (v_operate == "1D")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb1d.Reset();
				tqmtscb1d.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				tqmtscb1d.Delete("COMPOSE_LIST_NO2,DATE_C,MAT_CODE");

			}
		}
		if (v_operate == "1E")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb1d.Reset();
				tqmtscb1d.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				tqmtscb1d["STATUS_DESC"] = "3";
				tqmtscb1d["REC_REVISOR"] = s.userid;
				tqmtscb1d["REC_REVISE_TIME"] = datetime;
				tqmtscb1d.TrimOrBlank();
				tqmtscb1d.Update("REC_REVISOR,REC_REVISE_TIME,STATUS_DESC", "COMPOSE_LIST_NO2");

			}
		}
		if (v_operate == "0B")
		{
			CString SeqNo = "";
			CString heat_no = "";
			CString v_creat_time = "";
			CDecimal conversion_alloy_wt = 0;
			CDecimal conversion_ok_wt = 0;
			CString resume_seq_no = "";
			EIClass bcls_ret3;
			EIClass bcls_rec3;
			bcls_rec3.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
			
			bcls_rec3.Tables[0].Rows.Add();
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				
				//每次循环先将数据清空 在merge数据
				tmmsm0r91.Reset();
				tmmsm0r91.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				heat_no = tmmsm0r91["HEAT_NO"];
				conversion_alloy_wt = tmmsm0r91["CONVERSION_ALLOY_WT"];
				conversion_ok_wt = tmmsm0r91["CONVERSION_OK_WT"];
				resume_seq_no=tmmsm0r91["RESUME_SEQ_NO"].ToString().Trim();
				v_creat_time=tmmsm0r91["REC_CREATE_TIME"];
				if (resume_seq_no.Trim() == "")
				{
					sqlstr = "SELECT  LPAD(TO_CHAR(RESUME_SEQ_NO.NEXTVAL), 8, '0') FROM DUAL ";
					cmd_inqu.SetCommandText(sqlstr);
					cmd_inqu.ExecuteReader();
					if (cmd_inqu.Read())
					{
						SeqNo = cmd_inqu.GetString(1).Trim();
					}
					cmd_inqu.Close();
					resume_seq_no = v_creat_time + SeqNo;
				
					sqlstr = " update tmmsm0r91 set RESUME_SEQ_NO=@resume_seq_no "
					" where 1=1"
					" and conversion_ok_wt=@conversion_ok_wt"
					" and conversion_alloy_wt=@conversion_alloy_wt"
					" and heat_no=@heat_no"
					;
				    cmd_inq.SetCommandText(sqlstr);
				    cmd_inq.Parameters.Set("resume_seq_no", resume_seq_no);
				    cmd_inq.Parameters.Set("conversion_alloy_wt", conversion_alloy_wt);
				    cmd_inq.Parameters.Set("conversion_ok_wt", conversion_ok_wt);
				    cmd_inq.Parameters.Set("heat_no", heat_no);
				    cmd_inq.ExecuteNonQuery();
				    cmd_inq.Close();
					tmmsm0r91["RESUME_SEQ_NO"] = resume_seq_no;
				}
				//取原始重量
				

				sqlstr = "select T.STEEL_WT from tmmsm0r91 T WHERE T.RESUME_SEQ_NO= @resume_seq_no";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("resume_seq_no", resume_seq_no);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					v_steel_wt = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();

				v_steel_wt_new = tmmsm0r91["STEEL_WT"];

				if (v_steel_wt_new != v_steel_wt)
				{
					
					tmmsm0r91["C_UPDATESIGN"] = "1";
					tmmsm0r91.Update("STEEL_WT,C_UPDATESIGN", "RESUME_SEQ_NO");

					tmmsmgy05.Reset();
					tmmsmgy05["HEAT_NO"] = tmmsm0r91["HEAT_NO"];
					tmmsmgy05["OUT_STEEL_WT"] = v_steel_wt_new;
					tmmsmgy05.Update("OUT_STEEL_WT", "HEAT_NO");


					tmmsmopr.Reset();
					tmmsmopr["RESUME_SEQ_NO"] = tmmsm0r91["RESUME_SEQ_NO"];
					//取原始修改记录
					sqlstr = "select T.BACKC2 from TMMSMOPR T WHERE T.RESUME_SEQ_NO= @resume_seq_no";
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("resume_seq_no", tmmsm0r91["RESUME_SEQ_NO"].ToString().Trim());
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						v_back2 = cmd_inq.GetString(1);
					}
					cmd_inq.Close();
					if (tmmsmopr.QueryCount("RESUME_SEQ_NO") > 0)
					{
						tmmsmopr["REC_REVISOR"] = s.userid;
						tmmsmopr["REC_REVISE_TIME"] = datetime;
						tmmsmopr["BACKC2"] = v_back2+"修改时间:" + datetime + ";修改前重量:" + v_steel_wt.ToString() + ";修改后重量:" + v_steel_wt_new.ToString();
						tmmsmopr.Update("BACKC2", "RESUME_SEQ_NO");
					}
					else
					{
						tmmsmopr["REC_CREATOR"] = s.userid;
						tmmsmopr["REC_CREATE_TIME"] = datetime;
						tmmsmopr["RESUME_SEQ_NO"] = tmmsm0r91["RESUME_SEQ_NO"];
						tmmsmopr["EVENT_TIME"] = datetime;
						tmmsmopr["FUNC_ID"] = s.svc_name;
						tmmsmopr["CLIENT_IP"] = s.fore_ip;
						tmmsmopr["EVENT_CODE"] = "SENDZZ";
						tmmsmopr["EVENT_DESC"] = "发送ZZ";
						tmmsmopr["EVENT_NAME"] = "炉成本核算发送";
						tmmsmopr["BACKC2"] = v_back2 + "修改时间:" + datetime + ";修改前重量:" + v_steel_wt.ToString() + ";修改后重量:" + v_steel_wt_new.ToString();
						tmmsmopr.TrimOrBlank();
						tmmsmopr.Insert();
					}

					
				}
				
				bcls_rec3.Tables[0].Rows[0]["HEAT_NO"] = tmmsm0r91["HEAT_NO"];
				
				doFlag = f_mmsm_t80r91_snd(&bcls_rec3, &bcls_ret3, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_mmsm_t80r91_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				
			}
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}

