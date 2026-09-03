/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-17
Description: 试样成分实绩后备-试样成分查询
**************************************************/

//框架公用头文件
#include "stdafx.h"

//程序用头文件




/*<remark>=========================================================
/// <summary>
///试样成分实绩后备-试样成分查询
/// <para>
/// 1.试样成分实绩后备-试样成分查询
/// </para>
/// <para>数据库表：TQMTS25(实绩_工序成分)					</para>
/// <para>主调用函数：前台TQMTS25S的F2查询					</para>
/// <para>需调用函数：										</para>
/// </summary>
/// <returns>  </returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(qmts25s_inqe)


int f_qmts25s_inqe(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CDecimal v_count;
	CDecimal dummy = 0;     /* 制造命令状态 */
	CString v_pono;

	CString sqlstr = "";
	CString elm_ok_spec = " ";
	CString s_factory_div = " ";
	/* 实体类定义 */
	CModel tqmts02("TQMTS02");
	CModel tqmts25("TQMTS25");
	CModel tep0002("TEP0002");
	CModel tqmts0x("TQMTS0X");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_ts02(conn);
	CDbCommand cmd_inq_ts25(conn);
	CDbCommand cmd_ep(conn);   //代码查询用

	try
	{
		//定义返回块的列名
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ELM_CODE");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ELM_NAME");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "MAIN_AIM");   //元素主试目标值
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "MAIN_MIN");   //元素主试最小值
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "MAIN_MAX");   //元素主试最大值
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "SPE_MIN");   //元素主试最小值
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "SPE_MAX");   //元素主试最大值
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ELM_ACT");    //元素实际值
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ELM_OK");     //成分合否标志
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ELM_VALID");  //成分有效标志, 前台计算用,0-无效

		//-----------------------------------------
		//取得单行传入信息
		tqmts25["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		tqmts25["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString().Trim();
		tqmts25["WHOLE_BACKLOG_CODE"] = bcls_rec->Tables[0].Rows[0]["WHOLE_BACKLOG_CODE"].ToString().Trim();

		Log::Trace("", "", "qmts25s_inqe IN:---heat_no = [{0}]", (const char*)tqmts25["HEAT_NO"].ToString());
		Log::Trace("", "", "qmts25s_inqe IN:---st_sample_no = [{0}]", (const char*)tqmts25["ST_SAMPLE_NO"].ToString());
		Log::Trace("", "", "qmts25s_inqe IN:---whole_backlog_code = [{0}]", (const char*)tqmts25["WHOLE_BACKLOG_CODE"].ToString());

		//校验传入参数
		if (tqmts25["HEAT_NO"].ToString().TrimOrBlank() == " ")
		{
			strcpy(s.msg, _RES("QM00S0004334")/*熔炼号不允许为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tqmts25["WHOLE_BACKLOG_CODE"].ToString().TrimOrBlank() == " ")
		{
			strcpy(s.msg, _RES("QM00S0004177")/*请选择或输入工序代码。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//----------------------------------------------------------
		//查询炉次钢种管理表，得到出钢记号
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT ST_NO,FACTORY_DIV FROM"
				" (SELECT ST_NO,FACTORY_DIV FROM TPSSM11 WHERE HEAT_NO = @heat_no "
				" UNION "
				"  SELECT ST_NO,FACTORY_DIV FROM TPSSM41 WHERE HEAT_NO = @heat_no )"
				;
			break;
		}
		Log::Trace("", "", "sqlstr = [{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", tqmts25["HEAT_NO"].ToString().Trim());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tqmts02["ST_NO"] = cmd_inq.GetString(1).Trim();
			s_factory_div = cmd_inq.GetString(2).Trim().Substring(1, 1);
			Log::Trace("", __FUNCTION__, "111s_factory_div		= [{0}]", s_factory_div);

			if (s_factory_div == "1")
			{
				s_factory_div = "A";
			}
			else
			{
				s_factory_div = "B";
			}
		}
		cmd_inq.Close();
		tqmts0x["ST_NO"] = tqmts02["ST_NO"];
		tqmts0x["FACTORY_DIV"] = s_factory_div;
		if (tqmts0x.QueryCount("ST_NO,FACTORY_DIV") ==0)
		{
			s_factory_div = " ";  ////如果用特定的厂别查不到，就用空格查，update by yiling 20180425
		}
		Log::Trace("", "", "tqmts02.ST_NO = [{0}]s_factory_div[{1}]", (const char*)tqmts02["ST_NO"].ToString(), s_factory_div);
		if (tqmts02["ST_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, _RES("QM00S0004329")/*炉次钢种管理表的出钢记号为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//----------------------------------------------------------
		//查询指定试样号下的元素标准和实绩
		//1.按代码定义元素顺序取值
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT * "
				"  FROM TEP0002 "
				" WHERE CODE_CLASS = 'QMYS' "
				"   AND CODE_DESC_2_CONTENT!=' ' "
				" ORDER BY CODE_DESC_2_CONTENT ASC ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			
			cmd_inq.Fetch(tep0002);

			 
			Log::Trace("", "", "tep0002.code = [{0}]iiiiiii[{1}]",  tep0002["CODE"].ToString(),i);
			cmd_inq_ts02.Close();
			cmd_inq_ts25.Close();
			bcls_ret->Tables[0].Rows.Add();   //新增空行
			bcls_ret->Tables[0].Rows[i]["ELM_CODE"] = tep0002["CODE"];
			bcls_ret->Tables[0].Rows[i]["ELM_NAME"] = tep0002["CODE_DESC_1_CONTENT"];
			bcls_ret->Tables[0].Rows[i]["ELM_ACT"] = -1;//初试值
			bcls_ret->Tables[0].Rows[i]["MAIN_AIM"] = -1;//初试值
			bcls_ret->Tables[0].Rows[i]["MAIN_MAX"] = -1;//初试值
			bcls_ret->Tables[0].Rows[i]["MAIN_MIN"] = -1;//初试值
			bcls_ret->Tables[0].Rows[i]["SPE_MAX"] = -1;//初试值
			bcls_ret->Tables[0].Rows[i]["SPE_MIN"] = -1;//初试值

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT * "
					"  FROM TQMTS25 "
					" WHERE HEAT_NO = @heat_no "
					"   AND ST_SAMPLE_NO = @st_sample_no "
					"   AND ELM_CODE = @elm_code ";
				if (tqmts25["ST_SAMPLE_NO"].ToString().GetLength()>0 && tqmts25["ST_SAMPLE_NO"].ToString().Substring(1, 1) == "4")
				{
					sqlstr += "	AND ELM_CODE in ('001','014','016')";
				}
				break;
			}
			cmd_inq_ts25.SetCommandText(sqlstr);
			cmd_inq_ts25.Parameters.Set("heat_no", tqmts25["HEAT_NO"].ToString().Trim());
			cmd_inq_ts25.Parameters.Set("st_sample_no", tqmts25["ST_SAMPLE_NO"].ToString().Trim());
			cmd_inq_ts25.Parameters.Set("elm_code", tep0002["CODE"].ToString().Trim());
			cmd_inq_ts25.ExecuteReader();
			if (cmd_inq_ts25.Read())
			{
				cmd_inq_ts25.Fetch(tqmts25);
				//bcls_ret->Tables[0].Rows.Add();   //新增空行
				Log::Trace("", "", "tqmts25.ELM_NAME = [{0}]", tqmts25["ELM_NAME"].ToString());
				Log::Trace("", "", "tqmts25.ELM_ACT= [{0}]", tqmts25["ELM_ACT"].ToDecimal().ToDouble());
				Log::Trace("", "", "tqmts25.ELM_OK = [{0}]", tqmts25["ELM_OK"].ToDecimal().ToDouble());
				bcls_ret->Tables[0].Rows[i]["ELM_CODE"] = tqmts25["ELM_CODE"];
				bcls_ret->Tables[0].Rows[i]["ELM_NAME"] = tqmts25["ELM_NAME"];
				bcls_ret->Tables[0].Rows[i]["ELM_ACT"] = tqmts25["ELM_ACT"];
				bcls_ret->Tables[0].Rows[i]["ELM_OK"] = tqmts25["ELM_OK"].ToDecimal().ToString();
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT * "
						"  FROM TQMTS02 "
						" WHERE ST_NO = @st_no "
						" AND FACTORY_DIV =@s_factory_div"
						"   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
						"   AND ELM_CODE = @code ";
					break;
				}
				cmd_inq_ts02.SetCommandText(sqlstr);
				cmd_inq_ts02.Parameters.Set("st_no", tqmts02["ST_NO"].ToString().Trim());
				cmd_inq_ts02.Parameters.Set("whole_backlog_code", tqmts25["WHOLE_BACKLOG_CODE"].ToString().Trim());
				cmd_inq_ts02.Parameters.Set("s_factory_div", s_factory_div.Trim());
				cmd_inq_ts02.Parameters.Set("code", tqmts25["ELM_CODE"].ToString().Trim());
				//cmd_inq_ts02.Parameters.Set("code", tep0002["CODE"].ToString().Trim());
				cmd_inq_ts02.ExecuteReader();
				if (cmd_inq_ts02.Read())
				{
					cmd_inq_ts02.Fetch(tqmts02);
					//bcls_ret->Tables[0].Rows.Add();   //新增空行
					bcls_ret->Tables[0].Rows[i]["ELM_CODE"] = tqmts02["ELM_CODE"];
					bcls_ret->Tables[0].Rows[i]["ELM_NAME"] = tqmts02["ELM_NAME"];
					bcls_ret->Tables[0].Rows[i]["MAIN_AIM"] = tqmts02["MAIN_AIM"];
					bcls_ret->Tables[0].Rows[i]["MAIN_MAX"] = tqmts02["MAIN_MAX"];
					bcls_ret->Tables[0].Rows[i]["MAIN_MIN"] = tqmts02["MAIN_MIN"];
					bcls_ret->Tables[0].Rows[i]["SPE_MAX"] = tqmts02["SPE_MAX"];
					bcls_ret->Tables[0].Rows[i]["SPE_MIN"] = tqmts02["SPE_MIN"];
					bcls_ret->Tables[0].Rows[i]["ELM_VALID"] = " ";
				}
				else
				{
					cmd_inq_ts02.Close();
					cmd_inq_ts02.SetCommandText(sqlstr);
					Log::Trace("", "", "sqlstr = [{0}]", sqlstr);
					cmd_inq_ts02.Parameters.Set("st_no", tqmts02["ST_NO"].ToString().Trim());
					cmd_inq_ts02.Parameters.Set("whole_backlog_code", "G");
					cmd_inq_ts02.Parameters.Set("s_factory_div", s_factory_div.Trim());
					cmd_inq_ts02.Parameters.Set("code", tqmts25["ELM_CODE"].ToString().Trim());
					cmd_inq_ts02.ExecuteReader();
					if (cmd_inq_ts02.Read())
					{
						cmd_inq_ts02.Fetch(tqmts02);
						//bcls_ret->Tables[0].Rows.Add();   //新增空行
						bcls_ret->Tables[0].Rows[i]["ELM_CODE"] = tqmts02["ELM_CODE"];
						bcls_ret->Tables[0].Rows[i]["ELM_NAME"] = tqmts02["ELM_NAME"];
						bcls_ret->Tables[0].Rows[i]["MAIN_AIM"] = tqmts02["MAIN_AIM"];
						bcls_ret->Tables[0].Rows[i]["MAIN_MAX"] = tqmts02["MAIN_MAX"];
						bcls_ret->Tables[0].Rows[i]["MAIN_MIN"] = tqmts02["MAIN_MIN"];
						bcls_ret->Tables[0].Rows[i]["SPE_MAX"] = tqmts02["SPE_MAX"];
						bcls_ret->Tables[0].Rows[i]["SPE_MIN"] = tqmts02["SPE_MIN"];
						bcls_ret->Tables[0].Rows[i]["ELM_VALID"] = " ";
					}
					cmd_inq_ts02.Close();

				}
				//i++;
			}
			else
			{
				cmd_inq_ts25.Close();
				//2.读取标准
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT * "
						"  FROM TQMTS02 "
						" WHERE ST_NO = @st_no "
						" AND FACTORY_DIV =@s_factory_div"
						"   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
						"   AND ELM_CODE = @code ";
					if (tqmts25["ST_SAMPLE_NO"].ToString().GetLength()>0 && tqmts25["ST_SAMPLE_NO"].ToString().Substring(1, 1) == "4")
					{
						//查询出错，暂时去掉 20220816
					//	sqlstr += "	AND ELM_CODE in ('001','014','016')";
					}
					break;
				}
				Log::Trace("", "", "查询02表");
				cmd_inq_ts02.SetCommandText(sqlstr);
				cmd_inq_ts02.Parameters.Set("st_no", tqmts02["ST_NO"].ToString().Trim());
				cmd_inq_ts02.Parameters.Set("whole_backlog_code", tqmts25["WHOLE_BACKLOG_CODE"].ToString().Trim());
				cmd_inq_ts02.Parameters.Set("s_factory_div", s_factory_div.Trim());
				cmd_inq_ts02.Parameters.Set("code", tep0002["CODE"].ToString().Trim());
				cmd_inq_ts02.ExecuteReader();
				if (cmd_inq_ts02.Read())
				{
					cmd_inq_ts02.Fetch(tqmts02);
					//bcls_ret->Tables[0].Rows.Add();   //新增空行
					bcls_ret->Tables[0].Rows[i]["ELM_CODE"] = tqmts02["ELM_CODE"];
					bcls_ret->Tables[0].Rows[i]["ELM_NAME"] = tqmts02["ELM_NAME"];
					bcls_ret->Tables[0].Rows[i]["MAIN_AIM"] = tqmts02["MAIN_AIM"];
					bcls_ret->Tables[0].Rows[i]["MAIN_MAX"] = tqmts02["MAIN_MAX"];
					bcls_ret->Tables[0].Rows[i]["MAIN_MIN"] = tqmts02["MAIN_MIN"];
					bcls_ret->Tables[0].Rows[i]["SPE_MAX"] = tqmts02["SPE_MAX"];
					bcls_ret->Tables[0].Rows[i]["SPE_MIN"] = tqmts02["SPE_MIN"];
					bcls_ret->Tables[0].Rows[i]["ELM_VALID"] = " ";

					//3.查询每个元素对应的实绩数据
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT * "
							"  FROM TQMTS25 "
							" WHERE HEAT_NO = @heat_no "
							"   AND ST_SAMPLE_NO = @st_sample_no "
							"   AND ELM_CODE = @elm_code ";
						break;
					}
					Log::Trace("", "", "查询25表");
					Log::Trace("", "", "元素[{0}]", bcls_ret->Tables[0].Rows[i]["ELM_CODE"].ToString());
					cmd_inq_ts25.SetCommandText(sqlstr);
					cmd_inq_ts25.Parameters.Set("heat_no", tqmts25["HEAT_NO"].ToString().Trim());
					cmd_inq_ts25.Parameters.Set("st_sample_no", tqmts25["ST_SAMPLE_NO"].ToString().Trim());
					cmd_inq_ts25.Parameters.Set("elm_code", tqmts02["ELM_CODE"].ToString().Trim());
					cmd_inq_ts25.ExecuteReader();
					if (cmd_inq_ts25.Read())
					{
						cmd_inq_ts25.Fetch(tqmts25);
						Log::Trace("", "", "tqmts25.ELM_ACT = [{0}]", tqmts25["ELM_ACT"].ToDecimal().ToDouble());
						Log::Trace("", "", "tqmts25.ELM_OK = [{0}]", tqmts25["ELM_OK"].ToDecimal().ToDouble());
						bcls_ret->Tables[0].Rows[i]["ELM_ACT"] = tqmts25["ELM_ACT"];
						bcls_ret->Tables[0].Rows[i]["ELM_OK"] = tqmts25["ELM_OK"].ToDecimal().ToString();
					}
					cmd_inq_ts25.Close();
					//i++;
				}
				else
				{
					//没查到用通用标准查一遍
					cmd_inq_ts02.Close();
					cmd_inq_ts02.SetCommandText(sqlstr);
					cmd_inq_ts02.Parameters.Set("st_no", tqmts02["ST_NO"].ToString().Trim());
					cmd_inq_ts02.Parameters.Set("whole_backlog_code", "G");
					cmd_inq_ts02.Parameters.Set("s_factory_div", s_factory_div.Trim());
					cmd_inq_ts02.Parameters.Set("code", tep0002["CODE"].ToString().Trim());
					cmd_inq_ts02.ExecuteReader();
					if (cmd_inq_ts02.Read())
					{
						cmd_inq_ts02.Fetch(tqmts02);
						//bcls_ret->Tables[0].Rows.Add();   //新增空行
						bcls_ret->Tables[0].Rows[i]["ELM_CODE"] = tqmts02["ELM_CODE"];
						bcls_ret->Tables[0].Rows[i]["ELM_NAME"] = tqmts02["ELM_NAME"];
						bcls_ret->Tables[0].Rows[i]["MAIN_AIM"] = tqmts02["MAIN_AIM"];
						bcls_ret->Tables[0].Rows[i]["MAIN_MAX"] = tqmts02["MAIN_MAX"];
						bcls_ret->Tables[0].Rows[i]["MAIN_MIN"] = tqmts02["MAIN_MIN"];
						bcls_ret->Tables[0].Rows[i]["SPE_MAX"] = tqmts02["SPE_MAX"];
						bcls_ret->Tables[0].Rows[i]["SPE_MIN"] = tqmts02["SPE_MIN"];
						bcls_ret->Tables[0].Rows[i]["ELM_VALID"] = " ";

						//3.查询每个元素对应的实绩数据
						switch (conn->DatabaseKind)
						{
						case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
						case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
						case DB_KIND_MSSQL:				// MS SQL Server数据库
						case DB_KIND_ORACLE:	        // Oracle 数据库
						default:						// 所有数据库适用，通用SQL语句
							sqlstr = "SELECT * "
								"  FROM TQMTS25 "
								" WHERE HEAT_NO = @heat_no "
								"   AND ST_SAMPLE_NO = @st_sample_no "
								"   AND ELM_CODE = @elm_code ";
							break;
						}
						Log::Trace("", "", "查询25表");
						Log::Trace("", "", "元素[{0}]", bcls_ret->Tables[0].Rows[i]["ELM_CODE"].ToString());
						cmd_inq_ts25.SetCommandText(sqlstr);
						cmd_inq_ts25.Parameters.Set("heat_no", tqmts25["HEAT_NO"].ToString().Trim());
						cmd_inq_ts25.Parameters.Set("st_sample_no", tqmts25["ST_SAMPLE_NO"].ToString().Trim());
						cmd_inq_ts25.Parameters.Set("elm_code", tqmts02["ELM_CODE"].ToString().Trim());
						cmd_inq_ts25.ExecuteReader();
						if (cmd_inq_ts25.Read())
						{
							cmd_inq_ts25.Fetch(tqmts25);
							Log::Trace("", "", "tqmts25.ELM_ACT = [{0}]", tqmts25["ELM_ACT"].ToDecimal().ToDouble());
							Log::Trace("", "", "tqmts25.ELM_OK = [{0}]", tqmts25["ELM_OK"].ToDecimal().ToDouble());
							bcls_ret->Tables[0].Rows[i]["ELM_ACT"] = tqmts25["ELM_ACT"];
							bcls_ret->Tables[0].Rows[i]["ELM_OK"] = tqmts25["ELM_OK"].ToDecimal().ToString();
						}
						cmd_inq_ts25.Close();
						//i++;
					}
					cmd_inq_ts02.Close();
				}
			}

			i++;
			////2.读取标准
			//switch (conn->DatabaseKind)
			//{
			//case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//case DB_KIND_MSSQL:				// MS SQL Server数据库
			//case DB_KIND_ORACLE:	        // Oracle 数据库
			//default:						// 所有数据库适用，通用SQL语句
			//	sqlstr = "SELECT * "
			//		"  FROM TQMTS02 "
			//		" WHERE ST_NO = @st_no "
			//		" AND FACTORY_DIV =@s_factory_div"
			//		"   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
			//		"   AND ELM_CODE = @code ";
			//	break;
			//}
			//cmd_inq_ts02.SetCommandText(sqlstr);
			//cmd_inq_ts02.Parameters.Set("st_no", tqmts02["ST_NO"].ToString().Trim());
			//cmd_inq_ts02.Parameters.Set("whole_backlog_code", tqmts25["WHOLE_BACKLOG_CODE"].ToString().Trim());
			//cmd_inq_ts02.Parameters.Set("s_factory_div", s_factory_div.Trim());
			//cmd_inq_ts02.Parameters.Set("code", tep0002["CODE"].ToString().Trim());
			//cmd_inq_ts02.ExecuteReader();
			//if (cmd_inq_ts02.Read())
			//{
			//	cmd_inq_ts02.Fetch(tqmts02);
			//	bcls_ret->Tables[0].Rows.Add();   //新增空行
			//	bcls_ret->Tables[0].Rows[i]["ELM_CODE"] = tqmts02["ELM_CODE"];
			//	bcls_ret->Tables[0].Rows[i]["ELM_NAME"] = tqmts02["ELM_NAME"];
			//	bcls_ret->Tables[0].Rows[i]["MAIN_AIM"] = tqmts02["MAIN_AIM"];
			//	bcls_ret->Tables[0].Rows[i]["MAIN_MAX"] = tqmts02["MAIN_MAX"];
			//	bcls_ret->Tables[0].Rows[i]["MAIN_MIN"] = tqmts02["MAIN_MIN"];
			//	bcls_ret->Tables[0].Rows[i]["SPE_MAX"] = tqmts02["SPE_MAX"];
			//	bcls_ret->Tables[0].Rows[i]["SPE_MIN"] = tqmts02["SPE_MIN"];
			//	bcls_ret->Tables[0].Rows[i]["ELM_VALID"] = " ";

			//	//3.查询每个元素对应的实绩数据
			//	switch (conn->DatabaseKind)
			//	{
			//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:				// MS SQL Server数据库
			//	case DB_KIND_ORACLE:	        // Oracle 数据库
			//	default:						// 所有数据库适用，通用SQL语句
			//		sqlstr = "SELECT * "
			//			"  FROM TQMTS25 "
			//			" WHERE HEAT_NO = @heat_no "
			//			"   AND ST_SAMPLE_NO = @st_sample_no "
			//			"   AND ELM_CODE = @elm_code ";
			//		break;
			//	}
			//	cmd_inq_ts25.SetCommandText(sqlstr);
			//	cmd_inq_ts25.Parameters.Set("heat_no", tqmts25["HEAT_NO"].ToString().Trim());
			//	cmd_inq_ts25.Parameters.Set("st_sample_no", tqmts25["ST_SAMPLE_NO"].ToString().Trim());
			//	cmd_inq_ts25.Parameters.Set("elm_code", tqmts02["ELM_CODE"].ToString().Trim());
			//	cmd_inq_ts25.ExecuteReader();
			//	if (cmd_inq_ts25.Read())
			//	{
			//		cmd_inq_ts25.Fetch(tqmts25);
			//		Log::Trace("", "", "tqmts25["ELM_ACT"] = [{0}]", tqmts25["ELM_ACT"].ToDecimal().ToDouble());
			//		Log::Trace("", "", "tqmts25["ELM_OK"] = [{0}]", tqmts25["ELM_OK"].ToDecimal().ToDouble());
			//		bcls_ret->Tables[0].Rows[i]["ELM_ACT"] = tqmts25["ELM_ACT"];
			//		bcls_ret->Tables[0].Rows[i]["ELM_OK"] = tqmts25["ELM_OK"].ToDecimal().ToString();
			//	}
			//	cmd_inq_ts25.Close();
			//	i++;

			//}
			//else/////用厂别读，读不到用‘ ’读，update by yiling 20170106
			//{
			//	cmd_inq_ts02.Close();
			//	switch (conn->DatabaseKind)
			//	{
			//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:				// MS SQL Server数据库
			//	case DB_KIND_ORACLE:	        // Oracle 数据库
			//	default:						// 所有数据库适用，通用SQL语句
			//		sqlstr = "SELECT * "
			//			"  FROM TQMTS02 "
			//			" WHERE ST_NO = @st_no "
			//			" AND FACTORY_DIV =' '"
			//			"   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
			//			"   AND ELM_CODE = @code ";
			//		break;
			//	}
			//	cmd_inq_ts02.SetCommandText(sqlstr);
			//	cmd_inq_ts02.Parameters.Set("st_no", tqmts02["ST_NO"].ToString().Trim());
			//	cmd_inq_ts02.Parameters.Set("whole_backlog_code", tqmts25["WHOLE_BACKLOG_CODE"].ToString().Trim());
			//	cmd_inq_ts02.Parameters.Set("s_factory_div", s_factory_div.Trim());
			//	cmd_inq_ts02.Parameters.Set("code", tep0002["CODE"].ToString().Trim());
			//	cmd_inq_ts02.ExecuteReader();
			//	if (cmd_inq_ts02.Read())
			//	{
			//		cmd_inq_ts02.Fetch(tqmts02);
			//		bcls_ret->Tables[0].Rows.Add();   //新增空行
			//		bcls_ret->Tables[0].Rows[i]["ELM_CODE"] = tqmts02["ELM_CODE"];
			//		bcls_ret->Tables[0].Rows[i]["ELM_NAME"] = tqmts02["ELM_NAME"];
			//		bcls_ret->Tables[0].Rows[i]["MAIN_AIM"] = tqmts02["MAIN_AIM"];
			//		bcls_ret->Tables[0].Rows[i]["MAIN_MAX"] = tqmts02["MAIN_MAX"];
			//		bcls_ret->Tables[0].Rows[i]["MAIN_MIN"] = tqmts02["MAIN_MIN"];
			//		bcls_ret->Tables[0].Rows[i]["SPE_MAX"] = tqmts02["SPE_MAX"];
			//		bcls_ret->Tables[0].Rows[i]["SPE_MIN"] = tqmts02["SPE_MIN"];
			//		bcls_ret->Tables[0].Rows[i]["ELM_VALID"] = " ";

			//		//3.查询每个元素对应的实绩数据
			//		switch (conn->DatabaseKind)
			//		{
			//		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//		case DB_KIND_MSSQL:				// MS SQL Server数据库
			//		case DB_KIND_ORACLE:	        // Oracle 数据库
			//		default:						// 所有数据库适用，通用SQL语句
			//			sqlstr = "SELECT * "
			//				"  FROM TQMTS25 "
			//				" WHERE HEAT_NO = @heat_no "
			//				"   AND ST_SAMPLE_NO = @st_sample_no "
			//				"   AND ELM_CODE = @elm_code ";
			//			break;
			//		}
			//		cmd_inq_ts25.SetCommandText(sqlstr);
			//		cmd_inq_ts25.Parameters.Set("heat_no", tqmts25["HEAT_NO"].ToString().Trim());
			//		cmd_inq_ts25.Parameters.Set("st_sample_no", tqmts25["ST_SAMPLE_NO"].ToString().Trim());
			//		cmd_inq_ts25.Parameters.Set("elm_code", tqmts02["ELM_CODE"].ToString().Trim());
			//		cmd_inq_ts25.ExecuteReader();
			//		if (cmd_inq_ts25.Read())
			//		{
			//			cmd_inq_ts25.Fetch(tqmts25);
			//			Log::Trace("", "", "tqmts25["ELM_ACT"] = [{0}]", tqmts25["ELM_ACT"].ToDecimal().ToDouble());
			//			Log::Trace("", "", "tqmts25["ELM_OK"] = [{0}]", tqmts25["ELM_OK"].ToDecimal().ToDouble());
			//			bcls_ret->Tables[0].Rows[i]["ELM_ACT"] = tqmts25["ELM_ACT"];
			//			bcls_ret->Tables[0].Rows[i]["ELM_OK"] = tqmts25["ELM_OK"].ToDecimal().ToString();
			//		}
			//		cmd_inq_ts25.Close();
			//		i++;
			//	}
			//}
		}
		cmd_inq.Close();
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
