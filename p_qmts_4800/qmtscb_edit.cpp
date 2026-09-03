/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-01-15
Description:  处置F3 F4 F7
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明


BM2F_ENTERACE(qmtscb_edit)
int f_mmsm_zxh03(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_qmtscb_edit(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString procDiv = "";
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
	CModel ttk0001("TTK0001");
	CModel tqmtscb1a("TQMTSCB11A_DR");
	CModel tqmtscb1b("TQMTSCB11B_DR");
	CModel tqmtscb1d("TQMTSCB11D_DR");
	CModel tmmsmgy05("TMMSMGY05");
	CModel tqmtscbw5("TMMSMW5");
	CModel tmmsm2a_yl2("TMMSM2A_YL2");
	CString v_operate = "";

	CString v_date = "";
	CString v_area = "";
	CString v_costvalue = "";
	

	CString v_mat_name = "";
	CString v_mat_price = "";
	CString v_ni_std_ana = "";
	CString v_cr_std_ana = "";
	CString	v_ni_add_plus = "";
	CString	v_cr_add_plus = "";
	CString v_cr_ni_unit_price = "";

	CString v_price_type = "";

	CString v_mat_class = "";
	CString v_suc_name = "";
	CString v_mat_class_big = "";
	CString v_big_class_name = "";
	CString v_is_use = "";

	CString v_grade_type = "";
	CString v_price = "";

	CString v_sign = "";
	CString v_wt_unit = "";
	CString v_mat_cr_ana = "";
	CString v_mat_ni_ana = "";
	CString	v_mat_mo_ana = "";
	
	CDecimal v_use_cr = 0;
	CString v_use_ni = "";
	CString v_use_mo = "";
	CString v_unit_price = "";
	CString v_type_convert = "";
	CString	v_suggest_cr = "";
	CString v_suggest_ni = "";
	CString	v_suggest_mo = "";
	CString v_cost_unit = "";
	CString	v_seq_no = "";

	CString	v_area1 = "";
	CString	v_area2 = "";
	CString	v_steel_type = "";
	CString	v_grade_type2 = "";
	CString	v_grade_type3 = "";
	CString	v_mat_type = "";

	CDecimal unit_cr = 0;
	CDecimal unit_ni = 0;
	CDecimal unit_mo = 0;
	CDecimal unit_fe = 0;
	CDbCommand cmd_inq(conn);
	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("PRO_DIV"))
		{
			v_operate = bcls_rec->Tables[0].Rows[0]["PRO_DIV"].ToString().Trim();
			Log::Trace("", "", "条件查询v_operate[{0}],", v_operate);
		}
		/*for (int i = 0; i < bcls_rec->Tables[0].Columns.get_Count(); i++)
		{
			Log::Trace("", "", "sw v_u se_cr[{0}],", bcls_rec->Tables[0].Columns[i].get_ColumnName());
		}*/
		if (v_operate == "00")
		{  			
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				Log::Trace("", "", "7v_use_c");
				//每次循环先将数据清空 在merge数据
				tqmtscb00.Reset();
				tqmtscb00.MergeFrom(bcls_rec->Tables[0].Rows[i]); 			


				tqmtscb00["REC_REVISOR"] = s.userid;
				tqmtscb00["REC_REVISE_TIME"] = datetime; 	
				tqmtscb00["PRICE_TYPE"] = " ";
				tqmtscb00.TrimOrBlank();  				

				tqmtscb00.Update("REC_REVISOR,REC_REVISE_TIME,USE_CR,USE_NI,USE_MO,TYPE_CONVERT,SUGGEST_CR,SUGGEST_NI,SUGGEST_MO,UNIT_PRICE", "KM_CODE,MAT_CODE,DATE_C,PRICE_TYPE");
				Log::Trace("", "", "8v_use_c");
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


		tqmtscb00["REC_REVISOR"] = s.userid;
		tqmtscb00["REC_REVISE_TIME"] = datetime;
		tqmtscb00["PRICE_TYPE"] = "BZ";
		tqmtscb00.TrimOrBlank();

		tqmtscb00.Update("REC_REVISOR,REC_REVISE_TIME,USE_CR,USE_NI,USE_MO,TYPE_CONVERT,SUGGEST_CR,SUGGEST_NI,SUGGEST_MO,UNIT_PRICE", "KM_CODE,MAT_CODE,DATE_C,PRICE_TYPE");
		Log::Trace("", "", "8v_use_c");
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
		if (v_operate == "01")
		{
			if (bcls_rec->Tables[0].Columns.Contains("DATE_C"))
			{
				v_date = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().Trim();
				
			}
			if (bcls_rec->Tables[0].Columns.Contains("AREA_CODE"))
			{
				v_area = bcls_rec->Tables[0].Rows[0]["AREA_CODE"].ToString().Trim();
				
			}
			if (bcls_rec->Tables[0].Columns.Contains("COST_VALUE"))
			{
				v_costvalue = bcls_rec->Tables[0].Rows[0]["COST_VALUE"].ToString().Trim();

			}

			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb01.Reset();
				tqmtscb01.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				
					tqmtscb01["REC_REVISOR"] = s.userid;
					tqmtscb01["REC_REVISE_TIME"] = datetime;
					tqmtscb01["DATE_C"] = v_date;
					tqmtscb01["AREA_CODE"] = v_area;
					tqmtscb01["COST_VALUE"] = v_costvalue;
					
					tqmtscb01.TrimOrBlank();
					tqmtscb01.Update("REC_REVISOR,REC_REVISE_TIME,COST_VALUE", "DATE_C,AREA_CODE,MAT_CODE");
			}

		}
		if (v_operate == "02")
		{
			
			if (bcls_rec->Tables[0].Columns.Contains("MAT_NAME_DR"))
			{
				v_mat_name = bcls_rec->Tables[0].Rows[0]["MAT_NAME_DR"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("MAT_PRICE_DR"))
			{
				v_mat_price = bcls_rec->Tables[0].Rows[0]["MAT_PRICE_DR"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("NI_STD_ANA"))
			{
				v_ni_std_ana = bcls_rec->Tables[0].Rows[0]["NI_STD_ANA"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("CR_STD_ANA"))
			{
				v_cr_std_ana = bcls_rec->Tables[0].Rows[0]["CR_STD_ANA"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("NI_ADD_PLUS"))
			{
				v_ni_add_plus = bcls_rec->Tables[0].Rows[0]["NI_ADD_PLUS"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("CR_ADD_PLUS"))
			{
				v_cr_add_plus = bcls_rec->Tables[0].Rows[0]["CR_ADD_PLUS"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("CR_NI_UNIT_PRICE"))
			{
				v_cr_ni_unit_price = bcls_rec->Tables[0].Rows[0]["CR_NI_UNIT_PRICE"].ToString().Trim();

			}
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb02.Reset();
				tqmtscb02.MergeFrom(bcls_rec->Tables[0].Rows[i]);


				tqmtscb02["REC_REVISOR"] = s.userid;
				tqmtscb02["REC_REVISE_TIME"] = datetime;
				tqmtscb02["MAT_NAME_DR"] = v_mat_name;
				tqmtscb02["MAT_PRICE_DR"] = v_mat_price;
				tqmtscb02["NI_STD_ANA"] = v_ni_std_ana;
				tqmtscb02["CR_STD_ANA"] = v_cr_std_ana;
				tqmtscb02["NI_ADD_PLUS"] = v_ni_add_plus;

				tqmtscb02["CR_ADD_PLUS"] = v_cr_add_plus;
				tqmtscb02["CR_NI_UNIT_PRICE"] = v_cr_ni_unit_price;
				tqmtscb02.TrimOrBlank();
				tqmtscb02.Update("REC_REVISOR,REC_REVISE_TIME,MAT_NAME_DR,MAT_PRICE_DR,NI_STD_ANA,CR_STD_ANA,NI_ADD_PLUS,CR_ADD_PLUS,CR_NI_UNIT_PRICE", "DATE_C,MAT_CODE_DR");
			}

		}
		if (v_operate == "03")
		{
			if (bcls_rec->Tables[0].Columns.Contains("MAT_NAME_DR"))
			{
				v_mat_name = bcls_rec->Tables[0].Rows[0]["MAT_NAME_DR"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("PRICE_TYPE"))
			{
				v_price_type = bcls_rec->Tables[0].Rows[0]["PRICE_TYPE"].ToString().Trim();

			}
			
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb03.Reset();
				tqmtscb03.MergeFrom(bcls_rec->Tables[0].Rows[i]);


				tqmtscb03["REC_REVISOR"] = s.userid;
				tqmtscb03["REC_REVISE_TIME"] = datetime;
				tqmtscb03["MAT_NAME_DR"] = v_mat_name;
				tqmtscb03["PRICE_TYPE"] = v_price_type;
				
				tqmtscb03.TrimOrBlank();
				tqmtscb03.Update("REC_REVISOR,REC_REVISE_TIME,MAT_NAME_DR,PRICE_TYPE", "DATE_C,MAT_CODE_DR");
			}

		}
		if (v_operate == "04")
		{
			if (bcls_rec->Tables[0].Columns.Contains("MAT_NAME_DR"))
			{
				v_mat_name = bcls_rec->Tables[0].Rows[0]["MAT_NAME_DR"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("MAT_CLASS_SUC"))
			{
				v_mat_class = bcls_rec->Tables[0].Rows[0]["MAT_CLASS_SUC"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("SUC_NAME"))
			{
				v_suc_name = bcls_rec->Tables[0].Rows[0]["SUC_NAME"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("MAT_CLASS_BIG"))
			{
				v_mat_class_big = bcls_rec->Tables[0].Rows[0]["MAT_CLASS_BIG"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("BIG_CLASS_NAME"))
			{
				v_big_class_name = bcls_rec->Tables[0].Rows[0]["BIG_CLASS_NAME"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("IS_USE"))
			{
				v_is_use = bcls_rec->Tables[0].Rows[0]["IS_USE"].ToString().Trim();

			}
			
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb04.Reset();
				tqmtscb04.MergeFrom(bcls_rec->Tables[0].Rows[i]);


				tqmtscb04["REC_REVISOR"] = s.userid;
				tqmtscb04["REC_REVISE_TIME"] = datetime;
				tqmtscb04["MAT_NAME_DR"] = v_mat_name;
				tqmtscb04["MAT_CLASS_SUC"] =v_mat_class;
				tqmtscb04["SUC_NAME"] = v_suc_name;
				tqmtscb04["MAT_CLASS_BIG"] = v_mat_class_big;
				tqmtscb04["BIG_CLASS_NAME"] = v_big_class_name;

				tqmtscb04["IS_USE"] = v_is_use;
				
				tqmtscb04.TrimOrBlank();
				tqmtscb04.Update("REC_REVISOR,REC_REVISE_TIME,MAT_NAME_DR,MAT_CLASS_SUC,SUC_NAME,MAT_CLASS_BIG,BIG_CLASS_NAME,IS_USE", "MAT_CODE_DR");
			}

		}
		if (v_operate == "05")
		{		
		
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb05.Reset();
				tqmtscb05.MergeFrom(bcls_rec->Tables[0].Rows[i]);


				tqmtscb05["REC_REVISOR"] = s.userid;
				tqmtscb05["REC_REVISE_TIME"] = datetime;
				
				tqmtscb05.TrimOrBlank();
				tqmtscb05.Update("REC_REVISOR,REC_REVISE_TIME,MAT_CODE_DR, PRICE,KM_CODE,MAT_NAME_DR,KM_NAME,ST_NO_DESC", "PRICE_DATE,ST_NO");
			}

		}
		if (v_operate == "06")
		{
			if (bcls_rec->Tables[0].Columns.Contains("SIGN_CLASS"))
			{
				v_sign = bcls_rec->Tables[0].Rows[0]["SIGN_CLASS"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("GRADE_TYPE1"))
			{
				v_grade_type = bcls_rec->Tables[0].Rows[0]["GRADE_TYPE1"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("MAT_NAME"))
			{
				v_mat_name = bcls_rec->Tables[0].Rows[0]["MAT_NAME"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("WT_UNIT"))
			{
				v_wt_unit = bcls_rec->Tables[0].Rows[0]["WT_UNIT"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("MAT_CR_ANA"))
			{
				v_mat_cr_ana = bcls_rec->Tables[0].Rows[0]["MAT_CR_ANA"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("MAT_NI_ANA"))
			{
				v_mat_ni_ana = bcls_rec->Tables[0].Rows[0]["MAT_NI_ANA"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("MAT_MO_ANA"))
			{
				v_mat_mo_ana = bcls_rec->Tables[0].Rows[0]["MAT_MO_ANA"].ToString().Trim();

			}

			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb06.Reset();
				tqmtscb06.MergeFrom(bcls_rec->Tables[0].Rows[i]);


				tqmtscb06["REC_REVISOR"] = s.userid;
				tqmtscb06["REC_REVISE_TIME"] = datetime;
				tqmtscb06["SIGN_CLASS"] = v_sign;
				tqmtscb06["GRADE_TYPE1"] = v_grade_type;
				tqmtscb06["MAT_NAME"] = v_mat_name;
				tqmtscb06["WT_UNIT"] = v_wt_unit;
				tqmtscb06["MAT_CR_ANA"] = v_mat_cr_ana;

				tqmtscb06["MAT_NI_ANA"] = v_mat_ni_ana;
				tqmtscb06["MAT_MO_ANA"] = v_mat_mo_ana;
				tqmtscb06.TrimOrBlank();
				tqmtscb06.Update("REC_REVISOR,REC_REVISE_TIME,SIGN_CLASS,GRADE_TYPE1,MAT_NAME,WT_UNIT,MAT_CR_ANA,MAT_NI_ANA,MAT_MO_ANA", "MAT_CODE,DATE_C");
			}

		}
		if (v_operate == "07")
		{
			if (bcls_rec->Tables[0].Columns.Contains("SIGN_CLASS"))
			{
				v_sign = bcls_rec->Tables[0].Rows[0]["SIGN_CLASS"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("GRADE_TYPE1"))
			{
				v_grade_type = bcls_rec->Tables[0].Rows[0]["GRADE_TYPE1"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("MAT_NAME"))
			{
				v_mat_name = bcls_rec->Tables[0].Rows[0]["MAT_NAME"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("WT_UNIT"))
			{
				v_wt_unit = bcls_rec->Tables[0].Rows[0]["WT_UNIT"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("MAT_CR_ANA"))
			{
				v_mat_cr_ana = bcls_rec->Tables[0].Rows[0]["MAT_CR_ANA"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("MAT_NI_ANA"))
			{
				v_mat_ni_ana = bcls_rec->Tables[0].Rows[0]["MAT_NI_ANA"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("MAT_MO_ANA"))
			{
				v_mat_mo_ana = bcls_rec->Tables[0].Rows[0]["MAT_MO_ANA"].ToString().Trim();

			}

			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb07.Reset();
				tqmtscb07.MergeFrom(bcls_rec->Tables[0].Rows[i]);


				tqmtscb07["REC_REVISOR"] = s.userid;
				tqmtscb07["REC_REVISE_TIME"] = datetime;
				tqmtscb07["SIGN_CLASS"] = v_sign;
				tqmtscb07["GRADE_TYPE1"] = v_grade_type;
				tqmtscb07["MAT_NAME"] = v_mat_name;
				tqmtscb07["WT_UNIT"] = v_wt_unit;
				tqmtscb07["MAT_CR_ANA"] = v_mat_cr_ana;

				tqmtscb07["MAT_NI_ANA"] = v_mat_ni_ana;
				tqmtscb07["MAT_MO_ANA"] = v_mat_mo_ana;
				tqmtscb07.TrimOrBlank();
				tqmtscb07.Update("REC_REVISOR,REC_REVISE_TIME,SIGN_CLASS,GRADE_TYPE1,MAT_NAME,WT_UNIT,MAT_CR_ANA,MAT_NI_ANA,MAT_MO_ANA", "MAT_CODE,I_YEAR");
			}

		}
		if (v_operate == "08")
		{
			if (bcls_rec->Tables[0].Columns.Contains("MAT_NAME_DR"))
			{
				v_mat_name = bcls_rec->Tables[0].Rows[0]["MAT_NAME_DR"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("MAT_TYPE_DESC"))
			{
				v_mat_type = bcls_rec->Tables[0].Rows[0]["MAT_TYPE_DESC"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("MAT_CLASS_DESC"))
			{
				v_mat_class = bcls_rec->Tables[0].Rows[0]["MAT_CLASS_DESC"].ToString().Trim();

			}
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb08.Reset();
				tqmtscb08.MergeFrom(bcls_rec->Tables[0].Rows[i]);


				tqmtscb08["REC_REVISOR"] = s.userid;
				tqmtscb08["REC_REVISE_TIME"] = datetime;
				tqmtscb08["MAT_NAME_DR"] = v_mat_name;
				tqmtscb08["MAT_TYPE_DESC"] = v_mat_type;
				tqmtscb08["MAT_CLASS_DESC"] = v_mat_class;
				tqmtscb08.TrimOrBlank();
				tqmtscb08.Update("REC_REVISOR,REC_REVISE_TIME,MAT_NAME_DR,MAT_TYPE_DESC,MAT_CLASS_DESC", "MAT_CODE_DR");
			}

		}
		if (v_operate == "09")
		{
			if (bcls_rec->Tables[0].Columns.Contains("AREA1"))
			{
				v_area1 = bcls_rec->Tables[0].Rows[0]["AREA1"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("AREA2"))
			{
				v_area2 = bcls_rec->Tables[0].Rows[0]["AREA2"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("STEEL_TYPE"))
			{
				v_steel_type = bcls_rec->Tables[0].Rows[0]["STEEL_TYPE"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("GRADE_TYPE2"))
			{
				v_grade_type2 = bcls_rec->Tables[0].Rows[0]["GRADE_TYPE2"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("GRADE_TYPE3"))
			{
				v_grade_type3 = bcls_rec->Tables[0].Rows[0]["GRADE_TYPE3"].ToString().Trim();

			}
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb09.Reset();
				tqmtscb09.MergeFrom(bcls_rec->Tables[0].Rows[i]);


				tqmtscb09["REC_REVISOR"] = s.userid;
				tqmtscb09["REC_REVISE_TIME"] = datetime;
				tqmtscb09["AREA1"] = v_area1;
				tqmtscb09["AREA2"] = v_area2;
				tqmtscb09["STEEL_TYPE"] = v_steel_type;
				tqmtscb09["GRADE_TYPE2"] = v_grade_type2;
				tqmtscb09["GRADE_TYPE3"] = v_grade_type2;
				tqmtscb09.TrimOrBlank();
				tqmtscb09.Update("REC_REVISOR,REC_REVISE_TIME,AREA1,AREA2,STEEL_TYPE,GRADE_TYPE2,GRADE_TYPE3,STEEL_GRADE_EAF", "STEEL_GRADE");
			}

		}
		if (v_operate == "10")
		{
			if (bcls_rec->Tables[0].Columns.Contains("WT_UNIT"))
			{
				v_wt_unit = bcls_rec->Tables[0].Rows[0]["WT_UNIT"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("COST_UNIT"))
			{
				v_cost_unit = bcls_rec->Tables[0].Rows[0]["COST_UNIT"].ToString().Trim();

			}
			if (bcls_rec->Tables[0].Columns.Contains("SEQ_NO"))
			{
				v_seq_no = bcls_rec->Tables[0].Rows[0]["SEQ_NO"].ToString().Trim();

			}
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb10.Reset();
				tqmtscb10.MergeFrom(bcls_rec->Tables[0].Rows[i]);


				tqmtscb10["REC_REVISOR"] = s.userid;
				tqmtscb10["REC_REVISE_TIME"] = datetime;
				tqmtscb10["WT_UNIT"] = v_wt_unit;
				tqmtscb10["COST_UNIT"] = v_cost_unit;
				tqmtscb10["SEQ_NO"] = v_seq_no;
				tqmtscb10.TrimOrBlank();
				tqmtscb10.Update("REC_REVISOR,REC_REVISE_TIME,WT_UNIT,COST_UNIT,SEQ_NO,MAT_CODE_DR,MAT_NAME_DR", "GRADE_TYPE1,PROCESS_ROUTE,ZB_DESC");
			}

		}
		if (v_operate == "11")
		{ 			
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				ttk0001.Reset();
				ttk0001.MergeFrom(bcls_rec->Tables[0].Rows[i]);	
				ttk0001["REC_REVISOR"] = s.userid;
				ttk0001["REC_REVISE_TIME"] = datetime; 				
				ttk0001.TrimOrBlank();
				ttk0001.Update("REC_REVISOR,REC_REVISE_TIME,MAT_CODE,MAT_NAME,TYPE_CONVERT,MAT_NAME_T,XY_FLAG", "MAT_CODE_T");
			}

		}
		if (v_operate == "1A")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb1a.Reset();
				tqmtscb1a.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			    tqmtscb1a["REC_REVISOR"] = s.userid;
				tqmtscb1a["REC_REVISE_TIME"] = datetime;
				tqmtscb1a.TrimOrBlank();
				tqmtscb1a.Update("REC_REVISOR,REC_REVISE_TIME,KM_NAME,MAT_CODE,MAT_NAME,CR_VALUE,NI_VALUE,MO_VALUE,WT_UNIT", "YEAR,YEAR_MON,ST_NO,KM_CODE");
			}
		}
		if (v_operate == "W5")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscbw5.Reset();
				tqmtscbw5.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				tqmtscbw5["REC_REVISOR"] = s.userid;
				tqmtscbw5["REC_REVISE_TIME"] = datetime;
				tqmtscbw5.TrimOrBlank();
				tqmtscbw5.Update("REC_REVISOR,REC_REVISE_TIME,VALIDE_FLAG", "MAT_CODE,ELM_CODE");
			}
		}
		if (v_operate == "DW")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tmmsm2a_yl2.Reset();
				tmmsm2a_yl2.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				tmmsm2a_yl2["REC_REVISOR"] = s.userid;
				tmmsm2a_yl2["REC_REVISE_TIME"] = datetime;
				tmmsm2a_yl2.TrimOrBlank();
				tmmsm2a_yl2.Update("REC_REVISOR,REC_REVISE_TIME,TOTAL_WT,RATE,FLAG1", "MAT_CODE");
			}
		}
		if (v_operate == "1B")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb1b.Reset();
				tqmtscb1b.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				
					tqmtscb1b["REC_REVISOR"] = s.userid;
					tqmtscb1b["REC_REVISE_TIME"] = datetime;
					tqmtscb1b.TrimOrBlank();

					tqmtscb1b.Update("REC_REVISOR,REC_REVISE_TIME,CR_PDI,NI_PDI,MO_PDI", "ST_NO");
			}
		}
		if (v_operate == "1D")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb1d.Reset();
				tqmtscb1d.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				
					tqmtscb1d["REC_REVISOR"] = s.userid;
					tqmtscb1d["REC_REVISE_TIME"] = datetime;
					tqmtscb1d.TrimOrBlank();
					tqmtscb1d.Update("REC_REVISOR,REC_REVISE_TIME,DATE_TIME,GRADE_TYPE1,F_ROUTE1,AREA_CODE,YIELD,MAT_CODE_NAME,WEIGHT,C_VALUE,SI_VALUE,MN_VALUE,P_VALUE,S_VALUE,CR_VALUE,NI_VALUE,MO_VALUE,CU_VALUE,CO_VALUE,MAT_CLASS_DESC,UNIT_PRICE,COST", "COMPOSE_LIST_NO2,DATE_C,MAT_CODE,SEQ_NO");
				
				
			}
		}
		if (v_operate == "1E")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tqmtscb1d.Reset();
				tqmtscb1d.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				tqmtscb1d["DATE_TIME"] = datetime;
				tqmtscb1d["STATUS_DESC"] = "2";
				tqmtscb1d["REC_REVISOR"] = s.userid;
				tqmtscb1d["REC_REVISE_TIME"] = datetime;
				tqmtscb1d.TrimOrBlank();
				tqmtscb1d.Update("REC_REVISOR,REC_REVISE_TIME,DATE_TIME,STATUS_DESC", "COMPOSE_LIST_NO2");


			}
		}
		if (v_operate == "1H")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//每次循环先将数据清空 在merge数据
				tmmsmgy05.Reset();
				tmmsmgy05.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				tmmsmgy05["COMPOSE_LIST_NO2"] =bcls_rec->Tables[1].Rows[0]["COMPOSE_LIST_NO2"].ToString().Trim();
				tmmsmgy05["UPDATE_TIME"] = datetime;
				tmmsmgy05["REC_REVISOR"] = s.userid;
				tmmsmgy05["REC_REVISE_TIME"] = datetime;
				Log::Trace("", "20250320", tmmsmgy05["COMPOSE_LIST_NO2"]);
				Log::Trace("", "20250320", tmmsmgy05["HEAT_NO"]);
				tmmsmgy05.TrimOrBlank();
				tmmsmgy05.Update("REC_REVISOR,REC_REVISE_TIME,COMPOSE_LIST_NO2,UPDATE_TIME", "HEAT_NO");


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

