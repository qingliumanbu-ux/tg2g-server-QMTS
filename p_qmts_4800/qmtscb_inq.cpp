/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-09-22
Description: 查询
**************************************************/
//框架头文件
#include "stdafx.h"


// service入口
BM2F_ENTERACE(qmtscb_inq)

int f_qmtscb_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString	v_area = "";
	CString v_date = "";
	CString v_matcode = "";
	CString v_matname = "";
	CString	v_gradetype = "";
	CString v_signclass = "";

	CString	v_km_code = "";
	CString v_type_convert = "";
	CString v_from = "";//开始时刻
	CString v_from1 = "";//开始时刻
	CString v_to1 = "";//开始时刻

	CString v_cast_div_no ="";
	CString v_steel_wt ="";
	CString v_mat_type= "";
	CString v_mat_class= "";
	CString v_mat_code_dr = "";
	CString v_mat_name_dr = "";
	CString v_steel_grade = "";
	CString v_area1 = "";
	CString v_area2 = "";
	CString v_steel_type = "";
	CString v_grade_type = "";
	CString v_grade_type3 = "";	
	CString v_process_route = "";
	CString v_zb_desc = "";
	CString price_type = " ";
	
	CModel ttk0001("TTK0001");
	CModel tqmtscb1a("TQMTSCB11A_DR");
	CModel tqmtscb1b("TQMTSCB11B_DR");
	CModel tqmtscb1c("TQMTSCB11C");
	CModel tqmtscb1d("TQMTSCB11D_DR");
	CModel tmmsm0r91("TMMSM0R91");
	CModel tmmsmgy05("TMMSMGY05");
	CModel tqmtscb05_dr("TQMTSCB05_DR");
	CModel tqmtscben("TQMTSCB01_EN");
	CModel tqmtscbw5("TMMSMW5");
	CModel tmmsm2a_yl2("TMMSM2A_YL2");
	//系统的分页类信息。
	CPageInfo pageInfo;
	CString v_operate = "";
	
	CDbCommand cmd_inq(conn);

	try
	{
		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 20000;
		}

		if (bcls_rec->Tables[0].Columns.Contains("PRO_DIV"))
		{
			v_operate = bcls_rec->Tables[0].Rows[0]["PRO_DIV"].ToString().Trim();
			Log::Trace("", "", "条件查询v_operate[{0}],", v_operate);
		}
		//--------------------------------
		//获取传入参数
		if (v_operate == "00")
		{
			if (bcls_rec->Tables[0].Columns.Contains("DATE_C"))
			{
				v_from = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().SubstringNE(0,6);
			}
			
			if (bcls_rec->Tables[0].Columns.Contains("KM_CODE"))
				v_km_code= bcls_rec->Tables[0].Rows[0]["KM_CODE"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE"))
				v_matcode = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("TYPE_CONVERT"))
				v_type_convert = bcls_rec->Tables[0].Rows[0]["TYPE_CONVERT"].ToString();
		
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TQMTSCB00_DR "
					"  WHERE 1=1 "
					" and price_type = @price_type"
					;

				sqlstr = "SELECT * FROM TQMTSCB00_DR"
					" WHERE 1=1"
					" and price_type = @price_type"
					;

				if (v_km_code != "")
				{
					sqlstr_temp += " AND KM_CODE	like '%'||@v_km_code||'%'";
				}
				if (v_matcode != "")
				{
					sqlstr_temp += " AND MAT_CODE	like '%'||@v_matcode||'%'";
				}
				if (v_type_convert != "")
				{
					sqlstr_temp += " AND TYPE_CONVERT	= @v_type_convert";
				}
				
				if (v_from.Trim() != "")
				{
					sqlstr += " AND DATE_C = @v_from";
				}
				
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY KM_CODE";
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}


			cmd_inq.Parameters.Set("price_type", price_type);
			cmd_inq.Parameters.Set("v_km_code", v_km_code);
			cmd_inq.Parameters.Set("v_matcode", v_matcode);
			cmd_inq.Parameters.Set("v_type_convert", v_type_convert);
			cmd_inq.Parameters.Set("v_from", v_from);
		}
		if (v_operate == "0A")
		{
			if (bcls_rec->Tables[0].Columns.Contains("DATE_C"))
			{
				v_from = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().SubstringNE(0, 6);
			}

			if (bcls_rec->Tables[0].Columns.Contains("KM_CODE"))
				v_km_code = bcls_rec->Tables[0].Rows[0]["KM_CODE"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE"))
				v_matcode = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("TYPE_CONVERT"))
				v_type_convert = bcls_rec->Tables[0].Rows[0]["TYPE_CONVERT"].ToString();
			
				price_type ="BZ";
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TQMTSCB00_DR "
					"  WHERE 1=1 "
					" and price_type = @price_type"
					;

				sqlstr = "SELECT * FROM TQMTSCB00_DR"
					" WHERE 1=1"
					" and price_type = @price_type"
					;

				if (v_km_code != "")
				{
					sqlstr_temp += " AND KM_CODE	like '%'||@v_km_code||'%'";
				}
				if (v_matcode != "")
				{
					sqlstr_temp += " AND MAT_CODE	like '%'||@v_matcode||'%'";
				}
				if (v_type_convert != "")
				{
					sqlstr_temp += " AND TYPE_CONVERT	= @v_type_convert";
				}

				if (v_from.Trim() != "")
				{
					sqlstr += " AND DATE_C = @v_from";
				}

				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY KM_CODE";
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}


			cmd_inq.Parameters.Set("price_type", price_type);
			cmd_inq.Parameters.Set("v_km_code", v_km_code);
			cmd_inq.Parameters.Set("v_matcode", v_matcode);
			cmd_inq.Parameters.Set("v_type_convert", v_type_convert);
			cmd_inq.Parameters.Set("v_from", v_from);
			Log::Trace("", "", "sql[{0}],", sqlstr);
		}
		if (v_operate == "01")
		{
		   if (bcls_rec->Tables[0].Columns.Contains("DATE_C"))
			v_date = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().SubstringNE(0,6);
		   if (bcls_rec->Tables[0].Columns.Contains("AREA_CODE"))
			v_area = bcls_rec->Tables[0].Rows[0]["AREA_CODE"].ToString();
		   if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE"))
			   v_matcode = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString();
		   switch (conn->DatabaseKind)
		   {
		   case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		   case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		   case DB_KIND_MSSQL:				// MS SQL Server数据库
		   case DB_KIND_ORACLE:	        // Oracle 数据库
		   default:

			   sqlstr_count = " SELECT COUNT(1) "
				   "   FROM TQMTSCB01_DR "
				   "  WHERE 1=1 "
				   ;

			   sqlstr = "SELECT * FROM TQMTSCB01_DR WHERE 1=1";

			   if (v_date != "")
			   {
				   sqlstr_temp += " AND DATE_C	= @v_date";
			   }
			   if (v_area != "")
			   {
				   sqlstr_temp += " AND AREA_CODE	= @v_area";
			   }
			   if (v_area != "")
			   {
				   sqlstr_temp += " AND MAT_CODE	= @ v_matcode";
			   }
			   sqlstr_count = sqlstr_count + sqlstr_temp;
			   sqlstr_temp += " ORDER BY  DATE_C";
			   sqlstr = sqlstr + sqlstr_temp;
			   break;
		   }


		   cmd_inq.Parameters.Set("v_area", v_area);
		   cmd_inq.Parameters.Set("v_date", v_date);
		   cmd_inq.Parameters.Set("v_matcode", v_matcode);
		}
		if (v_operate == "02")
		{
			if (bcls_rec->Tables[0].Columns.Contains("DATE_C"))
				v_date = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE_DR"))
				v_matcode = bcls_rec->Tables[0].Rows[0]["MAT_CODE_DR"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("MAT_NAME_DR"))
				v_matname = bcls_rec->Tables[0].Rows[0]["MAT_NAME_DR"].ToString();
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TQMTSCB02_DR "
					"  WHERE 1=1 "
					;

				sqlstr = "SELECT * FROM TQMTSCB02_DR WHERE 1=1";

				if (v_date != "")
				{
					sqlstr_temp += " AND DATE_C	= @v_date";
				}
				if (v_matcode != "")
				{
					sqlstr_temp += " AND MAT_CODE_DR	= @v_matcode";
				}
				if (v_matname != "")
				{
					sqlstr_temp += " AND MAT_NAME_DR	= @v_matname";
				}
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY  DATE_C";
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}
			cmd_inq.Parameters.Set("v_date", v_date);
			cmd_inq.Parameters.Set("v_matcode", v_matcode);
			cmd_inq.Parameters.Set("v_matname", v_matname);
		}
		if (v_operate == "03")
		{
			if (bcls_rec->Tables[0].Columns.Contains("DATE_C"))
				v_date = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE_DR"))
				v_matcode = bcls_rec->Tables[0].Rows[0]["MAT_CODE_DR"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("MAT_NAME_DR"))
				v_matname = bcls_rec->Tables[0].Rows[0]["MAT_NAME_DR"].ToString();
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TQMTSCB03_DR "
					"  WHERE 1=1 "
					;

				sqlstr = "SELECT * FROM TQMTSCB03_DR WHERE 1=1";

				if (v_date != "")
				{
					sqlstr_temp += " AND DATE_C	= @v_date";
				}
				if (v_matcode != "")
				{
					sqlstr_temp += " AND MAT_CODE_DR	= @v_matcode";
				}
				if (v_matname != "")
				{
					sqlstr_temp += " AND MAT_NAME_DR	= @v_matname";
				}
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY  DATE_C";
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}
			cmd_inq.Parameters.Set("v_date", v_date);
			cmd_inq.Parameters.Set("v_matcode", v_matcode);
			cmd_inq.Parameters.Set("v_matname", v_matname);

		}
		if (v_operate == "04")
		{
			if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE_DR"))
				v_matcode = bcls_rec->Tables[0].Rows[0]["MAT_CODE_DR"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("MAT_NAME_DR"))
				v_matname = bcls_rec->Tables[0].Rows[0]["MAT_NAME_DR"].ToString();
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TQMTSCB04_DR "
					"  WHERE 1=1 "
					;

				sqlstr = "SELECT * FROM TQMTSCB04_DR WHERE 1=1";

				if (v_matcode != "")
				{
					sqlstr_temp += " AND MAT_CODE_DR	= @v_matcode";
				}
				if (v_matname != "")
				{
					sqlstr_temp += " AND MAT_NAME_DR	= @v_matname";
				}
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY  MAT_CODE_DR";
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}
			cmd_inq.Parameters.Set("v_matcode", v_matcode);
			cmd_inq.Parameters.Set("v_matname", v_matname);
		}
		if (v_operate == "05")
		{
			tqmtscb05_dr.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TQMTSCB05_DR "
					"  WHERE 1=1 "
					;

				sqlstr = "SELECT * FROM TQMTSCB05_DR WHERE 1=1";

				if (tqmtscb05_dr["MAT_CODE_DR"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_CODE_DR	= @mat_code_dr";
				}
				if (tqmtscb05_dr["ST_NO"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND ST_NO	= @st_no";
				}				
				if (tqmtscb05_dr["GRADE_TYPE1"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND GRADE_TYPE1	= @grade_type1";
				}
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY  st_no";
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}
			cmd_inq.Parameters.Set("mat_code_dr", tqmtscb05_dr["MAT_CODE_DR"].ToString());
			cmd_inq.Parameters.Set("st_no", tqmtscb05_dr["ST_NO"].ToString());
			cmd_inq.Parameters.Set("grade_type1", tqmtscb05_dr["GRADE_TYPE1"].ToString());

		}
		if (v_operate == "06")
		{
			if (bcls_rec->Tables[0].Columns.Contains("DATE_C"))
				v_date = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("SIGN_CLASS"))
				v_signclass = bcls_rec->Tables[0].Rows[0]["SIGN_CLASS"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE"))
				v_matcode = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString();
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TQMTSCB06_DR "
					"  WHERE 1=1 "
					;

				sqlstr = "SELECT * FROM TQMTSCB06_DR WHERE 1=1";

				if (v_date != "")
				{
					sqlstr_temp += " AND DATE_C	= @v_date";
				}
				if (v_signclass != "")
				{
					sqlstr_temp += " AND SIGN_CLASS	= @v_signclass";
				}
				if (v_matcode != "")
				{
					sqlstr_temp += " AND MAT_CODE = @v_matcode";
				}
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY  DATE_C";
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}

			cmd_inq.Parameters.Set("v_date", v_date);
			cmd_inq.Parameters.Set("v_signclass", v_signclass);
			cmd_inq.Parameters.Set("v_matcode", v_matcode);
		}
		if (v_operate == "07")
		{
			if (bcls_rec->Tables[0].Columns.Contains("I_YEAR"))
				v_date = bcls_rec->Tables[0].Rows[0]["I_YEAR"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("SIGN_CLASS"))
				v_signclass = bcls_rec->Tables[0].Rows[0]["SIGN_CLASS"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE"))
				v_matcode = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString();
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TQMTSCB07_DR "
					"  WHERE 1=1 "
					;

				sqlstr = "SELECT * FROM TQMTSCB07_DR WHERE 1=1";

				if (v_date != "")
				{
					sqlstr_temp += " AND I_YEAR	= @v_date";
				}
				if (v_signclass != "")
				{
					sqlstr_temp += " AND SIGN_CLASS	= @v_signclass";
				}
				if (v_matcode != "")
				{
					sqlstr_temp += " AND MAT_CODE = @v_matcode";
				}
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY I_YEAR";
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}

			cmd_inq.Parameters.Set("v_date", v_date);
			cmd_inq.Parameters.Set("v_signclass", v_signclass);
			cmd_inq.Parameters.Set("v_matcode", v_matcode);
		}
		if (v_operate == "08")
		{
			if (bcls_rec->Tables[0].Columns.Contains("MAT_TYPE_DESC"))
				v_mat_type = bcls_rec->Tables[0].Rows[0]["MAT_TYPE_DESC"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("MAT_CLASS_DESC"))
				v_mat_class = bcls_rec->Tables[0].Rows[0]["MAT_CLASS_DESC"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE_DR"))
				v_mat_code_dr = bcls_rec->Tables[0].Rows[0]["MAT_CODE_DR"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("MAT_NAME_DR"))
				v_mat_name_dr = bcls_rec->Tables[0].Rows[0]["MAT_NAME_DR"].ToString();

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TQMTSCB08_DR "
					"  WHERE 1=1 "
					;

				sqlstr = "SELECT * FROM TQMTSCB08_DR WHERE 1=1";

				if (v_mat_type != "")
				{
					sqlstr_temp += " AND MAT_TYPE_DESC= @v_mat_type";
				}
				if (v_mat_class != "")
				{
					sqlstr_temp += " AND MAT_CLASS_DESC	= @v_mat_class";
				}
				if (v_mat_code_dr != "")
				{
					sqlstr_temp += " AND MAT_CODE_DR= @v_mat_code_dr";
				}
				if (v_mat_name_dr != "")
				{
					sqlstr_temp += " AND MAT_NAME_DR	like  '%'||@v_mat_name_dr||'%'";
				}
				
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY MAT_CODE_DR";
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}

			cmd_inq.Parameters.Set("v_mat_type", v_mat_type);
			cmd_inq.Parameters.Set("v_mat_class", v_mat_class);
			cmd_inq.Parameters.Set("v_mat_code_dr", v_mat_code_dr);
			cmd_inq.Parameters.Set("v_mat_name_dr", v_mat_name_dr);
		}
		if (v_operate == "09")
		{
			if (bcls_rec->Tables[0].Columns.Contains("STEEL_GRADE"))
				v_steel_grade = bcls_rec->Tables[0].Rows[0]["STEEL_GRADE"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("AREA1"))
				v_area1 = bcls_rec->Tables[0].Rows[0]["AREA1"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("AREA2"))
				v_area2 = bcls_rec->Tables[0].Rows[0]["AREA2"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("STEEL_TYPE"))
				v_steel_type = bcls_rec->Tables[0].Rows[0]["STEEL_TYPE"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("GRADE_TYPE3"))
				v_grade_type3 = bcls_rec->Tables[0].Rows[0]["GRADE_TYPE3"].ToString();
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TQMTSCB09_DR "
					"  WHERE 1=1 "
					;

				sqlstr = "SELECT * FROM TQMTSCB09_DR WHERE 1=1";

				if (v_steel_grade != "")
				{
					sqlstr_temp += " AND STEEL_GRADE	= @v_steel_grade";
				}
				if (v_area1 != "")
				{
					sqlstr_temp += " AND AREA1	= @v_area1";
				}
				if (v_area2 != "")
				{
					sqlstr_temp += " AND AREA2 = @v_area2";
				}
				if (v_steel_type != "")
				{
					sqlstr_temp += " AND STEEL_TYPE= @v_steel_type";
				}
				if (v_grade_type3 != "")
				{
					sqlstr_temp += " AND grade_type3= @v_grade_type3";
				}
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY STEEL_GRADE";
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}

			cmd_inq.Parameters.Set("v_steel_grade", v_steel_grade);
			cmd_inq.Parameters.Set("v_area1", v_area1);
			cmd_inq.Parameters.Set("v_area2", v_area2);
			cmd_inq.Parameters.Set("v_steel_type", v_steel_type);
			cmd_inq.Parameters.Set("v_grade_type3", v_grade_type3);
			
		}
		if (v_operate == "10")
		{
			if (bcls_rec->Tables[0].Columns.Contains("GRADE_TYPE1"))
				v_grade_type = bcls_rec->Tables[0].Rows[0]["GRADE_TYPE1"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("PROCESS_ROUTE"))
				v_process_route = bcls_rec->Tables[0].Rows[0]["PROCESS_ROUTE"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("ZB_DESC"))
				v_zb_desc = bcls_rec->Tables[0].Rows[0]["ZB_DESC"].ToString();
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TQMTSCB03_FX "
					"  WHERE 1=1 "
					;

				sqlstr = "SELECT * FROM TQMTSCB03_FX WHERE 1=1";

				if (v_grade_type != "")
				{
					sqlstr_temp += " AND GRADE_TYPE1	= @v_grade_type ";
				}
				if (v_process_route != "")
				{
					sqlstr_temp += " AND PROCESS_ROUTE	= @v_process_route";
				}
				if (v_zb_desc != "")
				{
					sqlstr_temp += " AND ZB_DESC = @v_zb_desc";
				}
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY SEQ_NO";
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}

			cmd_inq.Parameters.Set("v_grade_type", v_grade_type);
			cmd_inq.Parameters.Set("v_process_route", v_process_route);
			cmd_inq.Parameters.Set("v_zb_desc", v_zb_desc);
		}
		if (v_operate == "11")
		{
			ttk0001.MergeFrom(bcls_rec->Tables[0].Rows[0]);				
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM ttk0001 "
					"  WHERE 1=1 "
					;

				sqlstr = "SELECT * FROM ttk0001 WHERE 1=1";

				if (ttk0001["MAT_CODE_T"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_CODE_T	like '%'||@mat_code_t||'%' ";
				}
				if (ttk0001["MAT_NAME_T"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_NAME_T	like '%'||@mat_name_t||'%' ";
				}
				if (ttk0001["MAT_CODE"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_CODE	like '%'||@mat_code||'%' ";
				}
				if (ttk0001["MAT_NAME"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_NAME	like '%'||@mat_name||'%' ";
				}
				
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY MAT_CODE_T";
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}

			cmd_inq.Parameters.Set("mat_code_t", ttk0001["MAT_CODE_T"].ToString());
			cmd_inq.Parameters.Set("mat_name_t", ttk0001["MAT_NAME_T"].ToString());
			cmd_inq.Parameters.Set("mat_code", ttk0001["MAT_CODE"].ToString());
			cmd_inq.Parameters.Set("mat_name", ttk0001["MAT_NAME"].ToString());
		}
		if (v_operate == "1A")
		{
			
			tqmtscb1a.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TQMTSCB11A_DR "
					"  WHERE 1=1 "
					;

				sqlstr = "SELECT * FROM TQMTSCB11A_DR WHERE 1=1";
			
				
				if (tqmtscb1a["ST_NO"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND ST_NO	like '%'||@st_no||'%' ";
				}
				if (tqmtscb1a["KM_NAME"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND KM_NAME	like '%'||@wce_name||'%' ";
				}

				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY MAT_CODE";
				sqlstr = sqlstr + sqlstr_temp;
				Log::Trace("", "", "条件查询sqlstr[{0}],", sqlstr);

				break;
			}

			
			cmd_inq.Parameters.Set("st_no", tqmtscb1a["ST_NO"].ToString());
			cmd_inq.Parameters.Set("wce_name", tqmtscb1a["KM_NAME"].ToString());
			
			
		}
		if (v_operate == "EN")
		{
			if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			{
				v_from1 = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().SubstringNE(0, 8);
			}
			if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			{
				v_to1 = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8);
			}
			tqmtscben.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TQMTSCB01_EN "
					"  WHERE 1=1 "
					;

				sqlstr = "SELECT * FROM TQMTSCB01_EN WHERE 1=1";
				if (v_from1 != "")
				{
					sqlstr_temp += " AND rec_create_time>=@v_from1 ";
				}
				if (v_to1 != "")
				{
					sqlstr_temp += " AND rec_create_time<=@v_to1 ";
				}
				if (tqmtscben["ROUTE"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND ROUTE	like '%'||@ROUTE||'%' ";
				}
				if (tqmtscben["HEAT_NUMBER"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND HEAT_NUMBER	like '%'||@HEAT_NUMBER||'%' ";
				}
				if (tqmtscben["EN_PROJECT"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND EN_PROJECT	like '%'||@EN_PROJECT||'%' ";
				}

				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY rec_create_time";
				sqlstr = sqlstr + sqlstr_temp;
				Log::Trace("", "", "条件查询sqlstr[{0}],", sqlstr);

				break;
			}
			cmd_inq.Parameters.Set("v_from1", v_from1);
			cmd_inq.Parameters.Set("v_to1", v_to1);
			cmd_inq.Parameters.Set("ROUTE", tqmtscben["ROUTE"].ToString());
			cmd_inq.Parameters.Set("HEAT_NUMBER", tqmtscben["HEAT_NUMBER"].ToString());
			cmd_inq.Parameters.Set("EN_PROJECT", tqmtscben["EN_PROJECT"].ToString());


		}
		if (v_operate == "W5")
		{
			
			tqmtscbw5.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TMMSMW5 "
					"  WHERE 1=1 "
					;

				sqlstr = "SELECT * FROM TMMSMW5 WHERE 1=1";
				
				if (tqmtscbw5["MAT_CODE"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_CODE like '%'||@MAT_CODE||'%' ";
				}
				

				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY MAT_CODE ";
				sqlstr = sqlstr + sqlstr_temp;
				Log::Trace("", "", "条件查询sqlstr[{0}],", sqlstr);

				break;
			}
			
			cmd_inq.Parameters.Set("MAT_CODE", tqmtscbw5["MAT_CODE"].ToString());
			

		}
		if (v_operate == "DW")
		{

			tmmsm2a_yl2.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TMMSM2A_YL2 "
					"  WHERE 1=1 "
					;

				sqlstr = "SELECT * FROM TMMSM2A_YL2 WHERE 1=1";

				if (tmmsm2a_yl2["MAT_CODE"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_CODE like '%'||@MAT_CODE||'%' ";
				}


				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY MAT_CODE ";
				sqlstr = sqlstr + sqlstr_temp;
				Log::Trace("", "", "条件查询sqlstr[{0}],", sqlstr);

				break;
			}

			cmd_inq.Parameters.Set("MAT_CODE", tmmsm2a_yl2["MAT_CODE"].ToString());


		}
		if (v_operate == "1B")
		{
			tqmtscb1b.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TQMTSCB11B_DR "
					"  WHERE 1=1 "
					;

				sqlstr = "SELECT * FROM TQMTSCB11B_DR WHERE 1=1";

				if (tqmtscb1b["ST_NO"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND ST_NO like '%'||@steel_grade||'%' ";
				}
				
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY ST_NO";
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}

			cmd_inq.Parameters.Set("steel_grade", tqmtscb1b["ST_NO"].ToString());
			
		}
		if (v_operate == "1C")
		{
			if (bcls_rec->Tables[0].Columns.Contains("DATE_C"))
			{
				v_from = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().SubstringNE(0, 6);
			}
			tqmtscb1c.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM (select T.DATE_C,T.ST_NO,SUM(T.UNIT_PRICE*T.WT_UNIT) as UNIT_PRICE,SUM(T.UNIT_PRICE_XY) as UNIT_PRICE_XY from TQMTSCB11C t GROUP BY T.DATE_C,T.ST_NO) "
					"  WHERE 1=1 "
					;

				sqlstr = "SELECT * FROM (select T.DATE_C,T.ST_NO,round(SUM(T.UNIT_PRICE*T.WT_UNIT)) as UNIT_PRICE,round(SUM(T.UNIT_PRICE_XY)) as UNIT_PRICE_XY from TQMTSCB11C t GROUP BY T.DATE_C,T.ST_NO) WHERE 1=1";

				
				
				if (tqmtscb1c["ST_NO"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND ST_NO	like '%'||@st_no||'%' ";
				}
				if (v_from.Trim() != "")
				{
					sqlstr += " AND DATE_C = @v_from";
				}
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY ST_NO";
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}

			cmd_inq.Parameters.Set("st_no", tqmtscb1c["ST_NO"].ToString());
			cmd_inq.Parameters.Set("v_from", v_from);
		}
		if (v_operate == "1D")
		{
			if (bcls_rec->Tables[0].Columns.Contains("FROM_DAY"))
			{
				v_from1 = bcls_rec->Tables[0].Rows[0]["FROM_DAY"].ToString().SubstringNE(0,8);
			}
			if (bcls_rec->Tables[0].Columns.Contains("TO_DAY"))
			{
				v_to1 = bcls_rec->Tables[0].Rows[0]["TO_DAY"].ToString().SubstringNE(0, 8);
			}
			tqmtscb1d.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TQMTSCB11D_DR "
					"  WHERE 1=1 "
					;

				sqlstr = "SELECT * FROM TQMTSCB11D_DR WHERE 1=1";

				if (tqmtscb1d["COMPOSE_LIST_NO2"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND COMPOSE_LIST_NO2 like '%'||@compose_listno||'%' ";
				}
				if (tqmtscb1d["GRADE_TYPE1"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND GRADE_TYPE1 like '%'||@grade_type||'%' ";
				}
				if (tqmtscb1d["AREA_CODE"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND AREA_CODE like '%'||@area_code||'%' ";
				}
				if (tqmtscb1d["MAT_CODE"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_CODE like '%'||@mat_code||'%' ";
				}
				if (tqmtscb1d["F_ROUTE1"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND F_ROUTE1 like '%'||@f_route1||'%' ";
				}
				if (tqmtscb1d["STATUS_DESC"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND STATUS_DESC like '%'||@status_desc||'%' ";
				}
				if (v_from1.Trim() != "")
				{
					sqlstr += " AND DATE_C>=@v_from ";
				}
				if (v_to1.Trim() != "")
				{
					sqlstr += " AND DATE_C<=@v_to ";
				}
				Log::Trace("", "20250527", v_from1.Trim());
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY MAT_CODE";
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}

			cmd_inq.Parameters.Set("compose_listno", tqmtscb1d["COMPOSE_LIST_NO2"].ToString());
			cmd_inq.Parameters.Set("grade_type", tqmtscb1d["GRADE_TYPE1"].ToString());
			cmd_inq.Parameters.Set("area_code", tqmtscb1d["AREA_CODE"].ToString());
			cmd_inq.Parameters.Set("mat_code", tqmtscb1d["MAT_CODE"].ToString());
			cmd_inq.Parameters.Set("f_route1", tqmtscb1d["F_ROUTE1"].ToString());
			cmd_inq.Parameters.Set("status_desc", tqmtscb1d["STATUS_DESC"].ToString());
			cmd_inq.Parameters.Set("v_from", v_from1);
			cmd_inq.Parameters.Set("v_to", v_to1);
		}
		if (v_operate == "1E")
		{
			if (bcls_rec->Tables[0].Columns.Contains("FROM_TIME"))
			{
				v_from1 = bcls_rec->Tables[0].Rows[0]["FROM_TIME"].ToString().SubstringNE(0, 8);
			}
			if (bcls_rec->Tables[0].Columns.Contains("TO_TIME"))
			{
				v_to1 = bcls_rec->Tables[0].Rows[0]["TO_TIME"].ToString().SubstringNE(0, 8);
			}

			tqmtscb1d.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				

				sqlstr = "SELECT distinct COMPOSE_LIST_NO2,DATE_TIME,DATE_C,GRADE_TYPE1,F_ROUTE1,STATUS_DESC FROM TQMTSCB11D_DR WHERE 1=1";

				if (v_from1 != "")
				{
					sqlstr_temp += " AND DATE_C>=@v_from1 ";
				}
				if (v_to1 != "")
				{
					sqlstr_temp += " AND DATE_C<=@v_to1 ";
				}
				if (tqmtscb1d["COMPOSE_LIST_NO2"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND COMPOSE_LIST_NO2 like '%'||@compose_listno||'%' ";
				}
				if (tqmtscb1d["GRADE_TYPE1"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND GRADE_TYPE1 like '%'||@grade_type||'%' ";
				}
				if (tqmtscb1d["AREA_CODE"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND AREA_CODE like '%'||@area_code||'%' ";
				}
				if (tqmtscb1d["MAT_CODE"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_CODE like '%'||@mat_code||'%' ";
				}

				
				sqlstr_temp += " ORDER BY DATE_C desc";
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}

			cmd_inq.Parameters.Set("compose_listno", tqmtscb1d["COMPOSE_LIST_NO2"].ToString());
			cmd_inq.Parameters.Set("grade_type", tqmtscb1d["GRADE_TYPE1"].ToString());
			cmd_inq.Parameters.Set("area_code", tqmtscb1d["AREA_CODE"].ToString());
			cmd_inq.Parameters.Set("mat_code", tqmtscb1d["MAT_CODE"].ToString());
			cmd_inq.Parameters.Set("v_from1", v_from1);
			cmd_inq.Parameters.Set("v_to1", v_to1);
		}
		if (v_operate == "0B")
		{
			CString is_all_flag = "0";
			is_all_flag = bcls_rec->Tables[0].Rows[0]["IS_ALL_FLAG"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("STAT_DATE"))
			{
				v_from = bcls_rec->Tables[0].Rows[0]["STAT_DATE"].ToString().SubstringNE(0, 6);
			}
			if (bcls_rec->Tables[0].Columns.Contains("FROM_TIME"))
			{
				v_from1 = bcls_rec->Tables[0].Rows[0]["FROM_TIME"].ToString().SubstringNE(0, 8);
			}
			if (bcls_rec->Tables[0].Columns.Contains("TO_TIME"))
			{
				v_to1 = bcls_rec->Tables[0].Rows[0]["TO_TIME"].ToString().SubstringNE(0, 8);
			}
			if (bcls_rec->Tables[0].Columns.Contains("CAST_DIV_NO_1"))
			{
				v_cast_div_no = bcls_rec->Tables[0].Rows[0]["CAST_DIV_NO_1"].ToString();
			}
			if (bcls_rec->Tables[0].Columns.Contains("STEEL_WT"))
			{
				v_steel_wt = bcls_rec->Tables[0].Rows[0]["STEEL_WT"].ToString();
			}
			tmmsm0r91.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:


				if (is_all_flag == "1")
				{
					sqlstr = "SELECT * FROM (SELECT t.*,g.end_time FROM TMMSM0R91 t,tmmsmgy05 g  WHERE t.heat_no=g.heat_no) WHERE 1=1";
				}
				else
				{
					sqlstr = "SELECT * FROM (SELECT t.*,g.end_time FROM  (select t1.* from (SELECT t0.*, row_number() over (partition by HEAT_NO ORDER BY rec_create_time desc) max_heat from TMMSM0R91 t0) t1 where max_heat=1) t,tmmsmgy05 g  WHERE t.heat_no=g.heat_no) WHERE 1=1";
				}
				if (v_cast_div_no != "")
				{
					sqlstr_temp += " AND CAST_DIV_NO_1 like '%'||@cast_div_no||'%' ";
				}
				if (tmmsm0r91["HEAT_NO"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND HEAT_NO like '%'||@heat_no||'%' ";
				}
				/*if (tmmsm0r91["ST_NO"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND ST_NO like '%'||@st_no||'%' ";
				}*/
				if (v_from != "")
				{
					sqlstr_temp += " AND STAT_DATE like '%'||@v_from||'%' ";
				}
				if (v_from1 != "")
				{
					sqlstr_temp += " AND END_TIME>=@v_from1 ";
				}
				if (v_to1 != "")
				{
					sqlstr_temp += " AND END_TIME<=@v_to1 ";
				}
				if (tmmsm0r91["TD_NO_1"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND TD_NO_1 like '%'||@td_no||'%' ";
				}
				if (v_steel_wt != "")
				{
					sqlstr_temp += " AND STEEL_WT like '%'||@steel_wt||'%' ";
				}
				
				sqlstr_temp += " ORDER BY STEEL_WT";
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}

			cmd_inq.Parameters.Set("cast_div_no", tmmsm0r91["CAST_DIV_NO_1"].ToDecimal());
			cmd_inq.Parameters.Set("heat_no", tmmsm0r91["HEAT_NO"].ToString());
			cmd_inq.Parameters.Set("st_no", tmmsm0r91["ST_NO"].ToString());
			cmd_inq.Parameters.Set("v_from", v_from);
			cmd_inq.Parameters.Set("v_from1", v_from1);
			cmd_inq.Parameters.Set("v_to1", v_to1);
			cmd_inq.Parameters.Set("td_no", tmmsm0r91["TD_NO_1"].ToString());
			cmd_inq.Parameters.Set("steel_wt", tmmsm0r91["STEEL_WT"].ToDecimal());
			Log::Trace("", "", "SQL[{0}],", sqlstr);
		}
		if (v_operate == "1G")
		{
			if (bcls_rec->Tables[0].Columns.Contains("FROM_TIME"))
			{
				v_from1 = bcls_rec->Tables[0].Rows[0]["FROM_TIME"].ToString().SubstringNE(0, 8);
			}
			if (bcls_rec->Tables[0].Columns.Contains("TO_TIME"))
			{
				v_to1 = bcls_rec->Tables[0].Rows[0]["TO_TIME"].ToString().SubstringNE(0, 8);
			}

			tmmsmgy05.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:       // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:      // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:       // MS SQL Server数据库
			case DB_KIND_ORACLE:          // Oracle 数据库
			default:

				sqlstr = "select * from tmmsmgy05 t WHERE HEAT_NO like 'A%'";
				if (v_from1 != "")
				{
					sqlstr_temp += " AND start_time>=@v_from1 ";
				}
				if (v_to1 != "")
				{
					sqlstr_temp += " AND start_time<=@v_to1 ";
				}
				if (tmmsmgy05["ST_NO"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND ST_NO like '%'||@st_no||'%' ";
				}
				if (tmmsmgy05["F_ROUTE1"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND F_ROUTE1 like '%'||@f_route1||'%' ";
				}
				if (tmmsmgy05["HEAT_NO"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND HEAT_NO like '%'||@heat_no||'%' ";
				}


				sqlstr_temp += " ORDER BY start_time desc ";
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}
			cmd_inq.Parameters.Set("st_no", tmmsmgy05["ST_NO"].ToString());
			cmd_inq.Parameters.Set("f_route1", tmmsmgy05["F_ROUTE1"].ToString());
			cmd_inq.Parameters.Set("heat_no", tmmsmgy05["HEAT_NO"].ToString());
			cmd_inq.Parameters.Set("v_from1", v_from1);
			cmd_inq.Parameters.Set("v_to1", v_to1);
		}
		if (v_operate == "1H")
		{
			tmmsmgy05.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:       // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:      // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:       // MS SQL Server数据库
			case DB_KIND_ORACLE:          // Oracle 数据库
			default:
				sqlstr = "select * from"
					" (select COMPOSE_LIST_NO2, DATE_C, F_ROUTE1, GRADE_TYPE1, row_number() over(partition by F_ROUTE1, GRADE_TYPE1 order by DATE_C desc) as ROW_ID"
					" from (select COMPOSE_LIST_NO2, DATE_C, F_ROUTE1, GRADE_TYPE1"
					" from tqmtscb11d_dr t2"
					" where t2.F_ROUTE1 =@f_route1"
					" and t2.GRADE_TYPE1 ="
					"(select grade_type3 from tqmtscb09_dr where STEEL_GRADE =@st_no) GROUP BY COMPOSE_LIST_NO2, DATE_C, F_ROUTE1, GRADE_TYPE1)) t where t.ROW_ID<6 ";
				//sqlstr = "select distinct COMPOSE_LIST_NO2 from tqmtscb11d_dr t2 where t2.F_ROUTE1=@f_route1 ";
				//sqlstr = "select COMPOSE_LIST_NO2 from tqmtscb11d_dr t2 where t2.F_ROUTE1=@f_route1 and t2.GRADE_TYPE1 =(select grade_type3 from tqmtscb09_dr where STEEL_GRADE =@st_no)"
					//" and DATE_TIME in(select max(DATE_TIME) from tqmtscb11d_dr t3 where t3.F_ROUTE1 =@f_route1 and t3.GRADE_TYPE1 = (select grade_type3 from tqmtscb09_dr where STEEL_GRADE =@st_no)"
					//" and date_time <= (select min(start_time) from tmmsmgy06 t4 where t4.handle_div = 'Y' and t4.heat_no =@heat_no))";
				
				break;
			}
			cmd_inq.Parameters.Set("st_no", tmmsmgy05["ST_NO"].ToString());
			cmd_inq.Parameters.Set("f_route1", tmmsmgy05["F_ROUTE1"].ToString());
			//cmd_inq.Parameters.Set("heat_no", tmmsmgy05["HEAT_NO"].ToString());
			Log::Trace("", "20250320", tmmsmgy05["HEAT_NO"]);
			Log::Trace("", "", "SQL[{0}],", sqlstr);
		}
		TotalRecordCount =1;
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();

		//返回分页总数量信息
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;

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
