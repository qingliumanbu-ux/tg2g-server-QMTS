/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-11
Description: 删除工艺卡
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


//#include "tqmts01.h"


#ifdef _SYS_MES











#endif
/*<remark>=========================================================
/// <summary>
/// 删除工艺卡
/// <para>
/// 1.删除工艺卡和对应的成分标准信息
/// </para>
/// <para>数据库表：TQMTS0X(工艺卡表);
//                  TQMTS02(工序成分标准表)                </para>
/// <para>主调用函数：前台QMTS0X画面的F5(删除)调用。       </para>
/// </summary>
/// <param name="ST_NO"> 出钢记号                          </param>
===========================================================</remark>*/

/* ***** 外部函数申明 ***** */
int f_pssm_stno_chk_using(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn); //校验出钢记号是否在计划中使用
int f_qm002001_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);

// service入口
BM2F_ENTERACE(qmts0x_del)

int f_qmts0x_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	CString base_code = " ";

	CDecimal flag = 0;
	CDecimal i_count = 0;
	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "FACTORY_DIV");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[1].Columns.Add(DT_STRING, "ST_IDX_NO");

	/* 实体类定义 */
	CModel tqmts0x("TQMTS0X");
	CModel hqmts0x("HQMTS0X");
	CModel tqmts02("TQMTS02");
	CModel hqmts02("HQMTS02");

	CModel tqmts01("TQMTS01");
	CModel tqmts03("TQMTS03");
	CModel tqmts04("TQMTS04");
	CModel tqmts05("TQMTS05");
	CModel tqmts05p("TQMTS05P");
	CModel tqmts06("TQMTS06");
	CModel tqmts07("TQMTS07");
	CModel tqmts08("TQMTS08");
	CModel tqmts0a("TQMTS0A");
	CModel tqmts0c("TQMTS0C");
	CModel tqmts0l("TQMTS0L");
	CModel tqmts0m("TQMTS0M");

	CString sqlstr("");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_B(conn);
	try
	{
		/*获得传入参数*/
		flag = bcls_rec->Tables[0].Rows[0]["FLAG"].ToDecimal();
		base_code = bcls_rec->Tables[0].Rows[0]["base_code"].ToString().Trim();
		tqmts0x["BASE_CODE"] = base_code;
		tqmts02["BASE_CODE"] = base_code;
		tqmts0x["ST_NO"] = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		/////字符型DB2数据库是个null, oracle是个空格，update by yiling 20160620,主键不好改成动态SQL查询
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容	
			tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
			break;
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
			break;
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank();
			break;
		}
		Log::Trace("", "", "qmts0x_del IN:---FLAG = [{0}]", flag.ToInt32());
		Log::Trace("", "", "qmts0x_ins IN:---BASE_CODE = [{0}]", base_code.Trim());
		Log::Trace("", "", "qmts0x_del IN:---ST_NO = [{0}]", (const char*)tqmts0x["ST_NO"].ToString());
		Log::Trace("", "", "qmts0x_ins IN:---FACTORY_DIV = [{0}]", (const char*)tqmts0x["FACTORY_DIV"].ToString());
		//厂别区分不再写死，前台如果传空，则表示不区分厂别，如果传了值，按前台的设定赋值
		//tqmts0x["FACTORY_DIV"] = "A";

		//校验出钢记号是否已审核——add by 冯晓轶 2012-04-26
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT VALID_FLAG "
				"   FROM TQMTS0X "
				"  WHERE FACTORY_DIV = @tqmts0x.FACTORY_DIV "
				"    AND ST_NO = @tqmts0x.ST_NO "
				"    AND base_code = @base_code ";

			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tqmts0x.FACTORY_DIV", tqmts0x["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("base_code", base_code);
		cmd_inq.Parameters.Set("tqmts0x.ST_NO", tqmts0x["ST_NO"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tqmts0x["VALID_FLAG"] = cmd_inq.GetString(1);
		}
		cmd_inq.Close();
		if (tqmts0x["VALID_FLAG"].ToString() == "1")
		{
			strcpy(s.msg, _RES("QM00S0005917")/*该出钢记号已审核，不可删除。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//调用检查出钢记号是否在计划中使用的函数——add by 冯晓轶 2012-03-23
		bcls_rec_s.Tables[0].Rows.Add();
		bcls_rec_s.Tables[0].Rows[0]["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
		bcls_rec_s.Tables[1].Rows.Add();
		bcls_rec_s.Tables[1].Rows[0]["ST_IDX_NO"] = tqmts0x["ST_NO"];

		//doFlag = f_pssm_stno_chk_using(&bcls_rec_s, bcls_ret,conn);

		if (doFlag != 0)
		{
			Log::Trace("", "", "f_pssm_stno_chk_using REEOR");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (flag == 0)  //删除工艺卡所有信息
		{
			//写工艺卡历史表——add by 冯晓轶 2012-03-23
			Log::Trace("", "", "INSERT HQMTS0X");
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT * "
					"   FROM TQMTS0X "
					"  WHERE FACTORY_DIV = @tqmts0x.FACTORY_DIV "
					"    AND ST_NO = @tqmts0x.ST_NO "
					"    AND base_code = @base_code ";

				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("tqmts0x.FACTORY_DIV", tqmts0x["FACTORY_DIV"].ToString());
			cmd_inq.Parameters.Set("base_code", base_code);
			cmd_inq.Parameters.Set("tqmts0x.ST_NO", tqmts0x["ST_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				cmd_inq.Fetch(hqmts0x);
			}
			cmd_inq.Close();
			hqmts0x["DU_FLAG"] = "D";
			hqmts0x["DU_MAKER"] = s.userid;
			hqmts0x["DU_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			hqmts0x.TrimOrBlank();
			hqmts0x.Insert();

			//删除工艺卡表
			Log::Trace("", "", "DELETE TQMTS0X");

			tqmts0x.Delete("FACTORY_DIV,ST_NO,BASE_CODE");

			//tqmts0x.Delete("FACTORY_DIV,ST_NO");

			/////////挂索引形式删除工艺卡时，同时删一遍制造标准表update by yiling 20160912
			tqmts01["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts03["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts04["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts05["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts05p["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts06["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts07["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts08["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts0a["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts0c["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts0l["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts0m["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts01["ST_NO"] = tqmts0x["ST_NO"];
			tqmts03["ST_NO"] = tqmts0x["ST_NO"];
			tqmts04["ST_NO"] = tqmts0x["ST_NO"];
			tqmts05["ST_NO"] = tqmts0x["ST_NO"];
			tqmts05p["ST_NO"] = tqmts0x["ST_NO"];
			tqmts06["ST_NO"] = tqmts0x["ST_NO"];
			tqmts07["ST_NO"] = tqmts0x["ST_NO"];
			tqmts08["ST_NO"] = tqmts0x["ST_NO"];
			tqmts0a["ST_NO"] = tqmts0x["ST_NO"];
			tqmts0c["ST_NO"] = tqmts0x["ST_NO"];
			tqmts0l["ST_NO"] = tqmts0x["ST_NO"];
			tqmts0m["ST_NO"] = tqmts0x["ST_NO"];
			tqmts01["BASE_CODE"] = base_code;
			tqmts03["BASE_CODE"] = base_code;
			tqmts04["BASE_CODE"] = base_code;
			tqmts05["BASE_CODE"] = base_code;
			tqmts05p["BASE_CODE"] = base_code;
			tqmts06["BASE_CODE"] = base_code;
			tqmts07["BASE_CODE"] = base_code;
			tqmts08["BASE_CODE"] = base_code;
			tqmts0a["BASE_CODE"] = base_code;
			tqmts0c["BASE_CODE"] = base_code;
			tqmts0l["BASE_CODE"] = base_code;
			tqmts0m["BASE_CODE"] = base_code;
			//if (base_code.Trim() != "")
			//	{
			tqmts01.Delete("FACTORY_DIV,ST_NO,BASE_CODE");
			tqmts03.Delete("FACTORY_DIV,ST_NO,BASE_CODE");
			tqmts04.Delete("FACTORY_DIV,ST_NO,BASE_CODE");
			tqmts05.Delete("FACTORY_DIV,ST_NO,BASE_CODE");
			tqmts05p.Delete("FACTORY_DIV,ST_NO,BASE_CODE");
			tqmts06.Delete("FACTORY_DIV,ST_NO,BASE_CODE");
			tqmts07.Delete("FACTORY_DIV,ST_NO,BASE_CODE");
			tqmts08.Delete("FACTORY_DIV,ST_NO,BASE_CODE");
			tqmts0a.Delete("FACTORY_DIV,ST_NO,BASE_CODE");
			tqmts0c.Delete("FACTORY_DIV,ST_NO,BASE_CODE");
			tqmts0l.Delete("FACTORY_DIV,ST_NO,BASE_CODE");
			tqmts0m.Delete("FACTORY_DIV,ST_NO,BASE_CODE");
			//}
			/*else
			{
			tqmts01.Delete("FACTORY_DIV,ST_NO");
			tqmts03.Delete("FACTORY_DIV,ST_NO");
			tqmts04.Delete("FACTORY_DIV,ST_NO");
			tqmts05.Delete("FACTORY_DIV,ST_NO");
			tqmts05p.Delete("FACTORY_DIV,ST_NO");
			tqmts06.Delete("FACTORY_DIV,ST_NO");
			tqmts07.Delete("FACTORY_DIV,ST_NO");
			tqmts08.Delete("FACTORY_DIV,ST_NO");
			tqmts0a.Delete("FACTORY_DIV,ST_NO");
			tqmts0c.Delete("FACTORY_DIV,ST_NO");
			tqmts0l.Delete("FACTORY_DIV,ST_NO");
			tqmts0m.Delete("FACTORY_DIV,ST_NO");
			}*/
			tqmts02["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts02["ST_NO"] = tqmts0x["ST_NO"];
			tqmts02["BASE_CODE"] = base_code;
			//tqmts02["WHOLE_BACKLOG_CODE"] = "G";  ////挂索引形式时，02表里有多个工序成分。全删除。
			//写成分标准历史表
			Log::Trace("", "", "INSERT HQMTS02");
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT * "
					"   FROM TQMTS02 "
					"  WHERE FACTORY_DIV = @tqmts02.FACTORY_DIV "
					//"    AND WHOLE_BACKLOG_CODE = @tqmts02.WHOLE_BACKLOG_CODE "
					"    AND ST_NO = @tqmts02.ST_NO ";
				if (base_code.Trim() != "")
				{
					sqlstr += "    AND base_code = @base_code ";
				}
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("tqmts02.FACTORY_DIV", tqmts02["FACTORY_DIV"].ToString());
			cmd_inq.Parameters.Set("base_code", base_code);
			cmd_inq.Parameters.Set("tqmts02.ST_NO", tqmts02["ST_NO"].ToString());
			//cmd_inq.Parameters.Set("tqmts02.WHOLE_BACKLOG_CODE", tqmts02["WHOLE_BACKLOG_CODE"].ToString());
			cmd_inq.ExecuteReader();

			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(hqmts02);
				hqmts02["DU_FLAG"] = "D";
				hqmts02["DU_MAKER"] = s.userid;
				hqmts02["DU_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				hqmts02.TrimOrBlank();
				hqmts02.Insert();
			}
			cmd_inq.Close();

			//删除工艺卡成分表
			Log::Trace("", "", "DELETE TQMTS02");

			tqmts02.Delete("FACTORY_DIV,ST_NO,BASE_CODE");

			//tqmts02.Delete("FACTORY_DIV,ST_NO,WHOLE_BACKLOG_CODE");
#ifdef _SYS_MMS
			//向PES发送工艺卡、成分标准同步电文
			Log::Trace("", "","SEND TC[002001]");
			doFlag = f_qm002001_snd(bcls_rec, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
#endif       
		}
		else if (flag == 1) //删除工艺卡成分标准
		{
			//对输入信息循环处理
			for (i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
			{
				//取得单行传入信息
				tqmts02.Reset();
				tqmts02.MergeFrom(bcls_rec->Tables[1].Rows[i]);

				if (tqmts02["ELM_CODE"].ToString().Trim() == "")
				{
					strcpy(s.msg, _RES("QM00S0004168")/*元素代码不能为空。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}

				tqmts02["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts02["ST_NO"] = tqmts0x["ST_NO"];
				tqmts02["WHOLE_BACKLOG_CODE"] = "G";
				tqmts02["BASE_CODE"] = base_code;

				//写成分标准历史表
				Log::Trace("", "", "INSERT HQMTS02");
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT COUNT(1)"
						"   FROM TQMTS02 "
						"  WHERE FACTORY_DIV = @tqmts02.FACTORY_DIV "
						"    AND ST_NO = @tqmts02.ST_NO "
						"    AND WHOLE_BACKLOG_CODE = @tqmts02.WHOLE_BACKLOG_CODE "
						"    AND ELM_CODE = @tqmts02.ELM_CODE "
						"    AND base_code = @base_code ";
					
					break;
				}
				cmd_inq_B.SetCommandText(sqlstr);
				cmd_inq_B.Parameters.Set("tqmts02.FACTORY_DIV", tqmts02["FACTORY_DIV"].ToString());
				cmd_inq_B.Parameters.Set("base_code", base_code);
				cmd_inq_B.Parameters.Set("tqmts02.ST_NO", tqmts02["ST_NO"].ToString());
				cmd_inq_B.Parameters.Set("tqmts02.WHOLE_BACKLOG_CODE", tqmts02["WHOLE_BACKLOG_CODE"].ToString());
				cmd_inq_B.Parameters.Set("tqmts02.ELM_CODE", tqmts02["ELM_CODE"].ToString());
				i_count = cmd_inq_B.ExecuteScalar();
				cmd_inq_B.Close();
				if (i_count != 0)
				{
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = " SELECT * "
							"   FROM TQMTS02 "
							"  WHERE FACTORY_DIV = @tqmts02.FACTORY_DIV "
							"    AND ST_NO = @tqmts02.ST_NO "
							"    AND WHOLE_BACKLOG_CODE = @tqmts02.WHOLE_BACKLOG_CODE "
							"    AND ELM_CODE = @tqmts02.ELM_CODE "
							"    AND base_code = @base_code ";
						
						break;
					}
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("tqmts02.FACTORY_DIV", tqmts02["FACTORY_DIV"].ToString());
					cmd_inq.Parameters.Set("base_code", base_code);
					cmd_inq.Parameters.Set("tqmts02.ST_NO", tqmts02["ST_NO"].ToString());
					cmd_inq.Parameters.Set("tqmts02.WHOLE_BACKLOG_CODE", tqmts02["WHOLE_BACKLOG_CODE"].ToString());
					cmd_inq.Parameters.Set("tqmts02.ELM_CODE", tqmts02["ELM_CODE"].ToString());
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						cmd_inq.Fetch(hqmts02);
					}
					cmd_inq.Close();
					hqmts02["DU_FLAG"] = "D";
					hqmts02["DU_MAKER"] = s.userid;
					hqmts02["DU_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
					hqmts02.TrimOrBlank();
					hqmts02.Insert();

					//删除工艺卡成分表
					Log::Trace("", "", "DELETE TQMTS02");

					tqmts02.Delete("FACTORY_DIV,ST_NO,WHOLE_BACKLOG_CODE,ELM_CODE,BASE_CODE");

					//tqmts02.Delete("FACTORY_DIV,ST_NO,WHOLE_BACKLOG_CODE,ELM_CODE");
				}
			}
		}
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
