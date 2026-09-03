/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-01-15
Description: 滞溜坯处置导入
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明


BM2F_ENTERACE(qmtscb_inexcel)
int f_mmsm_getprice(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_zxh03(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_qmtscb_inexcel(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	int seq_no = 0;
	int seq_no_tmp = 0;
	CString v_f_route1 = "";
	CString v_grade_type1 = "";
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
	CModel tqmtscb1a("TQMTSCB11A_DR");
	CModel tqmtscb1b("TQMTSCB11B_DR");
	CModel tqmtscb1d("TQMTSCB11D_DR");
	CModel tqmtscben("TQMTSCB01_EN");
	CModel tqmtscbw5("TMMSMW5");
	CModel ttk0001("TTK0001");
	CModel tmmsm2a_yl2("TMMSM2A_YL2");
	CString v_operate = "";

	CDecimal unit_cr = 0;
	CDecimal unit_ni = 0;
	CDecimal unit_mo = 0;
	CDecimal unit_fe = 0;
	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_max(conn);
	CDecimal cd_count = 0;
	/* 数据库操作类定义 */

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("PRO_DIV"))
		{
			v_operate = bcls_rec->Tables[0].Rows[0]["PRO_DIV"].ToString().Trim();
			Log::Trace("", "", "条件查询v_operate[{0}],", v_operate);
		}
		if (v_operate == "01")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb01.Reset();
				tqmtscb01.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				if (tqmtscb01.QueryCount("DATE_C,AREA_CODE,MAT_CODE")<1)
				{
					tqmtscb01["REC_CREATOR"] = s.userid;
					tqmtscb01["REC_CREATE_TIME"] = datetime;
					tqmtscb01["REC_REVISOR"] = " ";
					tqmtscb01["REC_REVISE_TIME"] = " ";
					tqmtscb01.TrimOrBlank();
					tqmtscb01.Insert();
				}
		        else
		        {
				   tqmtscb01["REC_REVISOR"] = s.userid;
				   tqmtscb01["REC_REVISE_TIME"] = datetime;
				   tqmtscb01.TrimOrBlank();
				   tqmtscb01.Update("*","DATE_C,AREA_CODE,MAT_CODE");
			  
		        }
			}
		}
		if (v_operate == "00")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb00.Reset();
				tqmtscb00.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				//根据成本科目取存货编码
				sqlstr = " select mat_code,mat_name,XY_FLAG"
					" from ttk0001"
					" where 1=1"
					" and mat_code_t = @km_code"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("km_code", tqmtscb00["KM_CODE"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tqmtscb00["MAT_CODE"] = cmd_inq.GetString(1);
					tqmtscb00["MAT_CODE_NAME"] = cmd_inq.GetString(2);
					tqmtscb00["XY_FLAG"] = cmd_inq.GetString(3);
				}
				cmd_inq.Close();
				if (tqmtscb00["MAT_CODE"].ToString().Trim() == "")
				{
					strcpy(s.msg, "成本科目" + tqmtscb00["KM_CODE"].ToString() + "未维护物料编码，请到【QMTSCB11S2N】 画面维护！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				
				tqmtscb00.Delete("KM_CODE,MAT_CODE,DATE_C,PRICE_TYPE");
				tqmtscb00["REC_CREATOR"] = s.userid;
				tqmtscb00["REC_CREATE_TIME"] = datetime;
				tqmtscb00.TrimOrBlank();
				tqmtscb00.Insert();					
			}

			if (bcls_rec->Tables[0].Rows.get_Count() > 0)
			{
				//计算效益铬钢单价、效益镍钢单价
				sqlstr = " select decode(USE_CR,0,0,round(UNIT_PRICE/USE_CR,3)) from tqmtscb00_dr"
					" where 1=1"
					" and PRICE_TYPE = ' '"
					" and mat_code = 'AT000329'"  //铬
					" and date_c = @date_c"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("date_c", bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					unit_cr = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();

				sqlstr = " select decode(USE_NI,0,0,round(UNIT_PRICE/USE_NI,3)) from tqmtscb00_dr"
					" where 1=1"
					" and PRICE_TYPE = ' '"
					" and mat_code = 'AT000385'"  //镍
					" and date_c = @date_c"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("date_c", bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					unit_ni = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();
				sqlstr = " select decode(USE_MO,0,0,round(UNIT_PRICE/USE_MO,3)) from tqmtscb00_dr"
					" where 1=1"
					" and PRICE_TYPE = ' '"
					" and mat_code = 'AT000331'"  //钼
					" and date_c = @date_c"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("date_c", bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					unit_mo = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();

				sqlstr = " select round(UNIT_PRICE/100,3) from tqmtscb00_dr"
					" where 1=1"
					" and PRICE_TYPE = ' '"
					" and mat_code = 'TS0000'"  //铁水
					" and date_c = @date_c"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("date_c", bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					unit_fe = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();

				//更新效益价格
				//更新效益价格
				sqlstr = " update tqmtscb00_dr set UNIT_PRICE_NI = round(USE_CR*@unit_cr+USE_NI*@unit_ni+USE_MO*@unit_mo-UNIT_PRICE,2)"
					",UNIT_PRICE_CR = round(USE_CR*@unit_cr+USE_NI*@unit_ni+USE_MO*@unit_mo+(99-USE_CR-USE_NI-USE_MO)*@unit_fe-UNIT_PRICE,2)"
					" where 1=1"
					" and PRICE_TYPE = ' '"
					" and XY_FLAG = '是'"
					" and date_c = @date_c"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("date_c", bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString());
				cmd_inq.Parameters.Set("unit_cr", unit_cr);
				cmd_inq.Parameters.Set("unit_ni", unit_ni);
				cmd_inq.Parameters.Set("unit_mo", unit_mo);
				cmd_inq.Parameters.Set("unit_fe", unit_fe);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				EIClass bcls_rec_xh;
				bcls_rec_xh.Tables[0].Columns.Add(DT_STRING, "STAT_DATE");
				bcls_rec_xh.Tables[0].Rows.Add();
				bcls_rec_xh.Tables[0].Rows[0]["STAT_DATE"] = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString();

				doFlag = f_mmsm_zxh03(&bcls_rec_xh, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				
			}
		}
		if (v_operate == "0A")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb00.Reset();
				tqmtscb00.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				tqmtscb00["PRICE_TYPE"] = "BZ";
				//根据成本科目取存货编码
				sqlstr = " select mat_code,mat_name,XY_FLAG"
					" from ttk0001"
					" where 1=1"
					//" and PRICE_TYPE = 'BZ'"
					" and mat_code_t = @km_code"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("km_code", tqmtscb00["KM_CODE"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tqmtscb00["MAT_CODE"] = cmd_inq.GetString(1);
					tqmtscb00["MAT_CODE_NAME"] = cmd_inq.GetString(2);
					tqmtscb00["XY_FLAG"] = cmd_inq.GetString(3);
				}
				cmd_inq.Close();
				if (tqmtscb00["MAT_CODE"].ToString().Trim() == "")
				{
					strcpy(s.msg, "成本科目" + tqmtscb00["KM_CODE"].ToString() + "未维护物料编码，请到【QMTSCB11S2N】 画面维护！");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				tqmtscb00.Delete("KM_CODE,MAT_CODE,DATE_C,PRICE_TYPE");
				tqmtscb00["REC_CREATOR"] = s.userid;
				tqmtscb00["REC_CREATE_TIME"] = datetime;
				tqmtscb00.TrimOrBlank();
				tqmtscb00.Insert();
			}

			if (bcls_rec->Tables[0].Rows.get_Count() > 0)
			{
				//计算效益铬钢单价、效益镍钢单价
				sqlstr = " select decode(USE_CR,0,0,round(UNIT_PRICE/USE_CR,3)) from tqmtscb00_dr"
					" where 1=1"
					" and PRICE_TYPE = 'BZ'"
					" and mat_code = 'AT000329'"  //铬
					" and date_c = @date_c"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("date_c", bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					unit_cr = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();

				sqlstr = " select decode(USE_NI,0,0,round(UNIT_PRICE/USE_NI,3)) from tqmtscb00_dr"
					" where 1=1"
					" and PRICE_TYPE = 'BZ'"
					" and mat_code = 'AT000385'"  //镍
					" and date_c = @date_c"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("date_c", bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					unit_ni = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();
				sqlstr = " select decode(USE_MO,0,0,round(UNIT_PRICE/USE_MO,3)) from tqmtscb00_dr"
					" where 1=1"
					" and PRICE_TYPE = 'BZ'"
					" and mat_code = 'AT000331'"  //钼
					" and date_c = @date_c"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("date_c", bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					unit_mo = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();

				sqlstr = " select round(UNIT_PRICE/100,3) from tqmtscb00_dr"
					" where 1=1"
					" and PRICE_TYPE = 'BZ'"
					" and mat_code = 'TS0000'"  //铁水
					" and date_c = @date_c"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("date_c", bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					unit_fe = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();

				//更新效益价格
				//更新效益价格
				sqlstr = " update tqmtscb00_dr set UNIT_PRICE_NI = round(USE_CR*@unit_cr+USE_NI*@unit_ni+USE_MO*@unit_mo-UNIT_PRICE,2)"
					",UNIT_PRICE_CR = round(USE_CR*@unit_cr+USE_NI*@unit_ni+USE_MO*@unit_mo+(99-USE_CR-USE_NI-USE_MO)*@unit_fe-UNIT_PRICE,2)"
					" where 1=1"
					" and PRICE_TYPE = 'BZ'"
					" and XY_FLAG = '是'"
					" and date_c = @date_c"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("date_c", bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString());
				cmd_inq.Parameters.Set("unit_cr", unit_cr);
				cmd_inq.Parameters.Set("unit_ni", unit_ni);
				cmd_inq.Parameters.Set("unit_mo", unit_mo);
				cmd_inq.Parameters.Set("unit_fe", unit_fe);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

			}
		}
		if (v_operate == "02")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb02.Reset();
				tqmtscb02.MergeFrom(bcls_rec->Tables[0].Rows[i]);				
				if (tqmtscb02.QueryCount("DATE_C,MAT_CODE_DR")<1)
				{
					tqmtscb02["REC_CREATOR"] = s.userid;
					tqmtscb02["REC_CREATE_TIME"] = datetime;
					tqmtscb02["REC_REVISOR"] = " ";
					tqmtscb02["REC_REVISE_TIME"] = " ";
					tqmtscb02.TrimOrBlank();
					tqmtscb02.Insert();
					Log::Trace("", "", "111");
				}
				else
				{
					tqmtscb02["REC_REVISOR"] = s.userid;
					tqmtscb02["REC_REVISE_TIME"] = datetime;
					tqmtscb02.TrimOrBlank();
					tqmtscb02.Update("*", "DATE_C,MAT_CODE_DR");

				}
			}
		}
		if (v_operate == "03")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb03.Reset();
				tqmtscb03.MergeFrom(bcls_rec->Tables[0].Rows[i]);				
				if (tqmtscb03.QueryCount("DATE_C,MAT_CODE_DR")<1)
				{
					tqmtscb03["REC_CREATOR"] = s.userid;
					tqmtscb03["REC_CREATE_TIME"] = datetime;
					tqmtscb03["REC_REVISOR"] = " ";
					tqmtscb03["REC_REVISE_TIME"] = " ";
					tqmtscb03.TrimOrBlank();
					tqmtscb03.Insert();
				}
				else
				{
					tqmtscb03["REC_REVISOR"] = s.userid;
					tqmtscb03["REC_REVISE_TIME"] = datetime;
					tqmtscb03.TrimOrBlank();
					tqmtscb03.Update("*", "DATE_C,MAT_CODE_DR");

				}
			}
		}
		if (v_operate == "04")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb04.Reset();
				tqmtscb04.MergeFrom(bcls_rec->Tables[0].Rows[i]);				
				if (tqmtscb04.QueryCount("MAT_CODE_DR")<1)
				{
					tqmtscb04["REC_CREATOR"] = s.userid;
					tqmtscb04["REC_CREATE_TIME"] = datetime;
					tqmtscb04["REC_REVISOR"] = " ";
					tqmtscb04["REC_REVISE_TIME"] = " ";
					tqmtscb04.TrimOrBlank();
					tqmtscb04.Insert();
				}
				else
				{
					tqmtscb04["REC_REVISOR"] = s.userid;
					tqmtscb04["REC_REVISE_TIME"] = datetime;
					tqmtscb04.TrimOrBlank();
					tqmtscb04.Update("*", "MAT_CODE_DR");

				}
			}
		}
		if (v_operate == "05")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb05.Reset();
				tqmtscb05.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				tqmtscb05["ST_NO"] = tqmtscb05["ST_NO_DESC"].ToString().SubstringNE(0, 6);
				tqmtscb05["MAT_CODE_DR"] = tqmtscb05["MAT_NAME_DR"].ToString().SubstringNE(0,5);
				tqmtscb05["KM_CODE"] = tqmtscb05["KM_NAME"].ToString().SubstringNE(0, 5);
				if (tqmtscb05.QueryCount("PRICE_DATE, ST_NO")<1)
				{
				  tqmtscb05["REC_CREATOR"] = s.userid;
				  tqmtscb05["REC_CREATE_TIME"] = datetime;
				  tqmtscb05["REC_REVISOR"] = " ";
				  tqmtscb05["REC_REVISE_TIME"] = " ";
				  tqmtscb05.TrimOrBlank();
				  tqmtscb05.Insert();
				}
				else
				{
					tqmtscb05["REC_REVISOR"] = s.userid;
					tqmtscb05["REC_REVISE_TIME"] = datetime;
					tqmtscb05.TrimOrBlank();
					tqmtscb05.Update("*", "PRICE_DATE,ST_NO");

				}
			}
		}
		if (v_operate == "06")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb06.Reset();
				tqmtscb06.MergeFrom(bcls_rec->Tables[0].Rows[i]);				
				if (tqmtscb06.QueryCount("MAT_CODE,DATE_C")<1)
				{
				  tqmtscb06["REC_CREATOR"] = s.userid;
				  tqmtscb06["REC_CREATE_TIME"] = datetime;
				  tqmtscb06["REC_REVISOR"] = " ";
				  tqmtscb06["REC_REVISE_TIME"] = " ";
				  tqmtscb06.TrimOrBlank();
				  tqmtscb06.Insert();
				}
				else
				{
					tqmtscb06["REC_REVISOR"] = s.userid;
					tqmtscb06["REC_REVISE_TIME"] = datetime;
					tqmtscb06.TrimOrBlank();
					tqmtscb06.Update("*", "MAT_CODE,DATE_C");

				}
			}
		}
		if (v_operate == "07")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb07.Reset();
				tqmtscb07.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			
				if (tqmtscb07.QueryCount("MAT_CODE,I_YEAR")<1)
				{
				  tqmtscb07["REC_CREATOR"] = s.userid;
				  tqmtscb07["REC_CREATE_TIME"] = datetime;
				  tqmtscb07["REC_REVISOR"] = " ";
				  tqmtscb07["REC_REVISE_TIME"] = " ";
				  tqmtscb07.TrimOrBlank();
				  tqmtscb07.Insert();
				}
				else
				{
					tqmtscb07["REC_REVISOR"] = s.userid;
					tqmtscb07["REC_REVISE_TIME"] = datetime;
					tqmtscb07.TrimOrBlank();
					tqmtscb07.Update("*", "MAT_CODE,I_YEAR");

				}
			}
		}
		if (v_operate == "08")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb08.Reset();
				tqmtscb08.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				
				if (tqmtscb08.QueryCount("MAT_CODE_DR")<1)
				{
				  tqmtscb08["REC_CREATOR"] = s.userid;
				  tqmtscb08["REC_CREATE_TIME"] = datetime;
				  tqmtscb08["REC_REVISOR"] = " ";
				  tqmtscb08["REC_REVISE_TIME"] = " ";
				  tqmtscb08.TrimOrBlank();
				  tqmtscb08.Insert();
				}
				else
				{
					tqmtscb08["REC_REVISOR"] = s.userid;
					tqmtscb08["REC_REVISE_TIME"] = datetime;
					tqmtscb08.TrimOrBlank();
					tqmtscb08.Update("*", "MAT_CODE_DR");

				}
			}
		}
		if (v_operate == "09")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb09.Reset();
				tqmtscb09.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				if (tqmtscb09.QueryCount("STEEL_GRADE")<1)
				{
				  tqmtscb09["REC_CREATOR"] = s.userid;
				  tqmtscb09["REC_CREATE_TIME"] = datetime;
				  tqmtscb09["REC_REVISOR"] = " ";
				  tqmtscb09["REC_REVISE_TIME"] = " ";
				  tqmtscb09.TrimOrBlank();
				  tqmtscb09.Insert();
				}
				else
				{
					tqmtscb09["REC_REVISOR"] = s.userid;
					tqmtscb09["REC_REVISE_TIME"] = datetime;
					tqmtscb09.TrimOrBlank();
					tqmtscb09.Update("*", "STEEL_GRADE");

				}
			}
		}
		if (v_operate == "10")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb10.Reset();
				tqmtscb10.MergeFrom(bcls_rec->Tables[0].Rows[i]);				
				if (tqmtscb10.QueryCount("GRADE_TYPE1,PROCESS_ROUTE,ZB_DESC")<1)
				{
				  tqmtscb10["REC_CREATOR"] = s.userid;
				  tqmtscb10["REC_CREATE_TIME"] = datetime;
				  tqmtscb10["REC_REVISOR"] = " ";
				  tqmtscb10["REC_REVISE_TIME"] = " ";
				  tqmtscb10.TrimOrBlank();
				  tqmtscb10.Insert();
				}
				else
				{
					tqmtscb10["REC_REVISOR"] = s.userid;
					tqmtscb10["REC_REVISE_TIME"] = datetime;
					tqmtscb10.TrimOrBlank();
					tqmtscb10.Update("*", "GRADE_TYPE1,PROCESS_ROUTE,ZB_DESC");

				}
			}
		}

		if (v_operate == "11")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				ttk0001.Reset();
				ttk0001.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				
				if (ttk0001.QueryCount("MAT_CODE,MAT_CODE_T")>0)
				{
					ttk0001["REC_REVISOR"] = s.userid;
					ttk0001["REC_REVISE_TIME"] = datetime;
					ttk0001.Update("REC_REVISOR,REC_REVISE_TIME,MAT_NAME,MAT_NAME_T,XY_FLAG,TYPE_CONVERT","MAT_CODE,MAT_CODE_T");
				}
				else
				{
					ttk0001["REC_CREATOR"] = s.userid;
					ttk0001["REC_CREATE_TIME"] = datetime;
					ttk0001.TrimOrBlank();
					ttk0001.Insert();
				}
				
			}
		}
		if (v_operate == "1A")
		{
			Log::Trace("", "", "条件查询get_Count[{0}],", bcls_rec->Tables[0].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb1a.Reset();
				tqmtscb1a.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				Log::Trace("", "", "st_no=[{0}],KM_CODE=[{1}]", tqmtscb1a["ST_NO"].ToString(), tqmtscb1a["KM_CODE"].ToString());
				//根据成本科目取存货编码
				sqlstr = " select mat_code,mat_name,mat_name_t"
					" from ttk0001"
					" where 1=1"
					//" and PRICE_TYPE = 'BZ'"
					" and mat_code_t = @km_code"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("km_code", tqmtscb1a["KM_CODE"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tqmtscb1a["MAT_CODE"] = cmd_inq.GetString(1);
					tqmtscb1a["MAT_NAME"] = cmd_inq.GetString(2);
					tqmtscb1a["KM_NAME"] = cmd_inq.GetString(3);
				}
				cmd_inq.Close();

				if (tqmtscb1a["KM_CODE"].ToString() == "199CR")
				{
					tqmtscb1a["MAT_CODE"] = "AT000329";
					tqmtscb1a["MAT_NAME"] = "高碳铬铁—FeCr50C10.0(自然块)国产";
				}
				if (tqmtscb1a["KM_CODE"].ToString() == "199NI")
				{
					tqmtscb1a["MAT_CODE"] = "AT000385";
					tqmtscb1a["MAT_NAME"] = "镍生铁-印尼（高镍）I级";
				}
				/*if (tqmtscb1a["KM_CODE"].ToString() == "199MO")
				{
					tqmtscb1a["MAT_CODE"] = " ";
					tqmtscb1a["MAT_NAME"] = " ";
				}*/
				
				if (tqmtscb1a["MAT_CODE"].ToString().Trim() == "")
				{
					strcpy(s.msg, "成本科目" + tqmtscb1a["KM_CODE"].ToString() + "未维护物料编码，请到【QMTSCB11S2N】 画面维护！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tqmtscb1a.QueryCount("YEAR,YEAR_MON,ST_NO,KM_CODE")>0)
				{
					tqmtscb1a["REC_REVISOR"] = s.userid;
					tqmtscb1a["REC_REVISE_TIME"] = datetime;
					tqmtscb1a.Update("REC_REVISOR,REC_REVISE_TIME,KM_NAME,MAT_CODE,MAT_NAME,CR_VALUE,NI_VALUE,MO_VALUE,WT_UNIT", "YEAR,YEAR_MON,ST_NO,KM_CODE");
				}
				else
				{
					tqmtscb1a["REC_CREATOR"] = s.userid;
					tqmtscb1a["REC_CREATE_TIME"] = datetime;
					tqmtscb1a.TrimOrBlank();
					tqmtscb1a.Insert();
				}

			}
			
		}
		if (v_operate == "1B")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb1b.Reset();
				tqmtscb1b.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				if (tqmtscb1b.QueryCount("ST_NO")>0)
				{
					tqmtscb1b["REC_REVISOR"] = s.userid;
					tqmtscb1b["REC_REVISE_TIME"] = datetime;
					tqmtscb1b.Update("REC_REVISOR,REC_REVISE_TIME,CR_PDI,NI_PDI,MO_PDI", "ST_NO");
				}
				else
				{
					tqmtscb1b["REC_CREATOR"] = s.userid;
					tqmtscb1b["REC_CREATE_TIME"] = datetime;
					tqmtscb1b.TrimOrBlank();
					tqmtscb1b.Insert();
				}

			}
		}
		if (v_operate == "EN")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscben.Reset();
				tqmtscben.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				tqmtscben["REC_CREATOR"] = s.userid;
				tqmtscben["REC_CREATE_TIME"] = datetime;
				tqmtscben.TrimOrBlank();
				tqmtscben.Insert();

			}
		}
		if (v_operate == "W5")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscbw5.Reset();
				tqmtscbw5.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				if (tqmtscbw5.QueryCount("MAT_CODE,ELM_CODE")>0)
				{
					tqmtscbw5["VALIDE_FLAG"] = '1';
					tqmtscbw5["REC_REVISOR"] = s.userid;
					tqmtscbw5["REC_REVISE_TIME"] = datetime;
					tqmtscbw5.Update("REC_REVISOR,REC_REVISE_TIME,VALIDE_FLAG", "MAT_CODE,ELM_CODE");
				}
				else
				{
					tqmtscbw5["VALIDE_FLAG"] = '1';
					tqmtscbw5["REC_CREATOR"] = s.userid;
					tqmtscbw5["REC_CREATE_TIME"] = datetime;
					tqmtscbw5.TrimOrBlank();
					tqmtscbw5.Insert();
				}
			}
		}
		if (v_operate == "DW")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tmmsm2a_yl2.Reset();
				tmmsm2a_yl2.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				if (tmmsm2a_yl2.QueryCount("MAT_CODE")>0)
				{
					
					tmmsm2a_yl2["REC_REVISOR"] = s.userid;
					tmmsm2a_yl2["REC_REVISE_TIME"] = datetime;
					tmmsm2a_yl2.Update("REC_REVISOR,REC_REVISE_TIME,TOTAL_WT,RATE,FLAG1", "MAT_CODE");
				}
				else
				{
				
					tmmsm2a_yl2["REC_CREATOR"] = s.userid;
					tmmsm2a_yl2["REC_CREATE_TIME"] = datetime;
					tmmsm2a_yl2.TrimOrBlank();
					tmmsm2a_yl2.Insert();
				}
			}
		}
		if (v_operate == "1D")
		{
			sqlstr = " SELECT "
				"nvl(MAX(SEQ_NO),0) as SEQ_NO "
				" FROM TQMTSCB11D_DR ";
			cmd_max.SetCommandText(sqlstr);
			
			cmd_max.ExecuteReader();
			if (cmd_max.Read())
			{
				seq_no = cmd_max.GetInt32(1);
				seq_no_tmp = seq_no;
				cmd_max.Close();
			}
			if (bcls_rec->Tables[1].Columns.Contains("GRADE_TYPE1"))
			{
				v_grade_type1 = bcls_rec->Tables[1].Rows[0]["GRADE_TYPE1"].ToString().Trim();
				Log::Trace("", "", "v_grade_type1[{0}],", v_grade_type1);
			}
			if (bcls_rec->Tables[1].Columns.Contains("F_ROUTE1"))
			{
				v_f_route1 = bcls_rec->Tables[1].Rows[0]["F_ROUTE1"].ToString().Trim();
				Log::Trace("", "", "v_f_route1[{0}],", v_f_route1);
			}
			if (v_grade_type1.GetLength() == 0 || v_f_route1.GetLength() == 0)
			{
				if (v_grade_type1.GetLength() == 0)
					{
						strcpy(s.msg, "钢种类型不能为空!");
					}
				if (v_f_route1.GetLength() == 0)
					{
						strcpy(s.msg, "生产工艺不能为空!");
					}
				throw CApplicationException(-1, s.msg, log.Location);
			}
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb1d.Reset();
				tqmtscb1d.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				tqmtscb1d["STATUS_DESC"] ="1";
				tqmtscb1d["DATE_C"] = CDateTime::Now().ToString("yyyyMMdd");
				tqmtscb1d["COMPOSE_LIST_NO2"] = datetime; 
				tqmtscb1d["SEQ_NO"] = seq_no_tmp + i+1;
				//更新物料价格
				EIClass bcls_ret3;
				EIClass bcls_rec3;
				bcls_rec3.Tables[0].Columns.Add(DT_STRING, "MAT_CODE");

				bcls_rec3.Tables[0].Columns.Add(DT_DECIMAL, "CR_VALUE");
				bcls_rec3.Tables[0].Columns.Add(DT_DECIMAL, "NI_VALUE");
				bcls_rec3.Tables[0].Columns.Add(DT_DECIMAL, "MO_VALUE");

				bcls_rec3.Tables[0].Rows.Add();


				bcls_rec3.Tables[0].Rows[0]["MAT_CODE"] = tqmtscb1d["MAT_CODE"];
				bcls_rec3.Tables[0].Rows[0]["CR_VALUE"] = tqmtscb1d["CR_VALUE"];
				bcls_rec3.Tables[0].Rows[0]["NI_VALUE"] = tqmtscb1d["NI_VALUE"];
				bcls_rec3.Tables[0].Rows[0]["MO_VALUE"] = tqmtscb1d["MO_VALUE"];
				doFlag = f_mmsm_getprice(&bcls_rec3, &bcls_ret3, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_mmsm_getprice失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				else
				{
					tqmtscb1d["UNIT_PRICE"]=bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE"].ToDecimal();
				}
				tqmtscb1d["COST"] = tqmtscb1d["UNIT_PRICE"].ToDecimal() * tqmtscb1d["WEIGHT"].ToDecimal();
				tqmtscb1d["GRADE_TYPE1"] = v_grade_type1;
				tqmtscb1d["F_ROUTE1"] = v_f_route1;
				tqmtscb1d["REC_CREATOR"] = s.userid;
				tqmtscb1d["REC_CREATE_TIME"] = datetime;
				tqmtscb1d.TrimOrBlank();
				tqmtscb1d.Insert();
				

				
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

