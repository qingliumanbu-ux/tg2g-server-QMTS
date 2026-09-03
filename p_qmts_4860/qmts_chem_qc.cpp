/* ****************************************************************************
*	Copyright (c) Baosight Corporation 2011 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*******************************************************************************
*  程序名称			: qmts_chem_qc
*  程序描述			: 处理期初材料的成分
*  备注说明			:
*  修改历史			:
*  2023/08/23 	    :(ADD)程序建立 by panchen
*			... ...
* **************************************************************************** */

#include <map>
#include "stdafx.h"

/***** service入口 **************/
BM2F_ENTERACE(qmts_chem_qc)
BM2_FUNCTION_EXPORT

int f_qmts_chem_qc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;
	int v_count = 0;

	CString now = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CModel tqmtsb0("TQMTSB0");
	CModel tqmts29("TQMTS29");
	CModel tep0002("TEP0002");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_upd(conn);
	int upd_num = 0;

	CString sqlstr = "";

	/*********** 保存元素和名称的对应关系 *********/
	map<CString, CString> map_code;

	/*********** 保存待处理信息 *******************/
	EIClass bcls_rec_qc;

	EIClass bcls_rec_f1;
	EIClass bcls_ret_f1;

	try
	{

		bcls_rec_f1.Tables[0].Columns.Add(DT_STRING, "SYS_CODE");
		bcls_rec_f1.Tables[0].Columns.Add(DT_STRING, "PONO");
		bcls_rec_f1.Tables[0].Rows.Add();

		s.flag = 0;
		s.sqlcode = 0;
		strcpy(s.msg, " ");

		/************ 建立元素代码和元素名称的HASHMAP **************/
		map_code.clear();
		sqlstr = " SELECT CODE,CODE_DESC_1_CONTENT FROM TEP0002 WHERE CODE_CLASS = 'QMYS2N' ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		fetchRowCount = 0;
		while (cmd_inq.Read())
		{
			fetchRowCount++;
			tep0002["CODE"] = cmd_inq.GetString(1);
			tep0002["CODE_DESC_1_CONTENT"] = cmd_inq.GetString(2);

			map_code.insert(pair<CString, CString>(tep0002["CODE"].ToString(), tep0002["CODE_DESC_1_CONTENT"].ToString()));
		}
		cmd_inq.Close();

		
		//

	/*	sqlstr = "TRUNCATE TABLE  PM_SAMPLE";
		cmd_upd.SetCommandText(sqlstr);
		cmd_upd.ExecuteNonQuery();
		cmd_upd.Close();

		sqlstr = "INSERT INTO  PM_SAMPLE  SELECT * FROM PM_SAMPLE_RES WHERE SAMPLETIME  >= to_date(q'/2023-03-01 00:00:00/', 'yyyy-mm-dd hh24:mi:ss')  ";
		cmd_upd.SetCommandText(sqlstr);
		cmd_upd.ExecuteNonQuery();
		cmd_upd.Close();*/

		
		/*********** 查询表中未处理的记录 *********************/
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			sqlstr = " SELECT SAMPLEID AS ST_SAMPLE_NO,"
"				HEATNUMBER AS HEAT_NO,"
"				HEATNUMBER AS PONO,"
"				AGGREGATECODE AS DEV_CODE,"
"				GRADEID AS ST_NO,"
"				ELEMENTCOUNT AS ELEMENTCOUNT,"
"				SAMPLENUMBER AS ST_SAMPLE_SEQ,"
"				C AS ELM_001,"
"				SI AS ELM_002,"
"				MN AS ELM_003,"
"				P AS ELM_004,"
"				S AS ELM_005,"
"				AL AS ELM_010,"
"				CU AS ELM_009,"
"				NI AS ELM_007,"
"				CR AS ELM_006,"
"				ARSENIC  AS ELM_021,"
"				SN AS ELM_023,"
"				NB AS ELM_011,"
"				V AS ELM_012,"
"				TI AS ELM_013,"
"				MO AS ELM_008,"
"				B AS ELM_018,"
"				W AS ELM_019,"
"				CA AS ELM_020,"
"				H AS ELM_014,"
"				O AS ELM_015,"
"				N AS ELM_016,"
"				CO AS ELM_017,"
"				ZR AS ELM_026,"
"				SB AS ELM_024,"
"				MG AS ELM_029,"
"				FE AS ELM_030,"
"				ZN AS ELM_114,"
"				SE AS ELM_113,"
"				TE AS ELM_031,"
"				PB AS ELM_022,"
"				BI AS ELM_025,"
"				CRNIEQ  AS ELM_041,"
"				ALS AS ELM_036,"
"				TA AS ELM_033"
"				FROM PM_SAMPLE"
"				WHERE  HEATNUMBER IN('A1401982','A2401902','A2401905','A2401906','A2401907','A2401934' "
") AND SAMPLEID IN('A1401982-C0T-1#-3', 'A2401902-C2T-1#-2','A2401905-C2T-1#-2','A2401906-C2T-1#-3','A2401907-C2T-1#-5','A2401934-C1T-1#-2') "
"				ORDER BY HEAT_NO ASC ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_rec_qc.Tables[0]);
		cmd_inq.Close();

		/************* 循环处理 ********************************/
		Log::Trace("", "", "待处理条数bcls_rec_qc.Tables[0].Rows.get_Count = [{0}]", bcls_rec_qc.Tables[0].Rows.get_Count());
		fetchRowCount = 0;
		for (int iii = 0; iii < bcls_rec_qc.Tables[0].Rows.get_Count(); iii++)
		{
			fetchRowCount++;
			tqmtsb0.MergeFrom(bcls_rec_qc.Tables[0].Rows[iii]);
			tqmtsb0["REC_CREATOR"] = "qc";
			tqmtsb0["REC_CREATE_TIME"] = s.datetime;
			tqmtsb0.TrimOrBlank();
			/************** 处理成分 *************************/
			/** 1-先校验是否已存在成分数据；                **/
			/** 2-删除数据；                                **/
			/** 3-处理数据；                                **/
			tqmtsb0["HEAT_NO"] = tqmtsb0["HEAT_NO"];
			if (tqmtsb0.QueryCount("HEAT_NO") > 0)
			{

				//20160922 add:前面已更新物料主档，后面提交的话导入主档表与成分表的炉号，PONO对应关系混乱，故先回滚
				CTransactionManager::Abort(0);
				CTransactionManager::Begin(0, 0);

				sqlstr = "UPDATE PM_SAMPLE  SET AGGREGATENAME = '2' WHERE HEATNUMBER = @heat_no ";
				cmd_upd.SetCommandText(sqlstr);
				cmd_upd.Parameters.Set("heat_no", tqmtsb0["HEAT_NO"]);
				cmd_upd.ExecuteNonQuery();
				CTransactionManager::Commit(0);
				CTransactionManager::Begin(0, 0);
				continue;
			}
			tqmtsb0.Delete("HEAT_NO");
			tqmts29["HEAT_NO"] = tqmtsb0["HEAT_NO"];
			tqmts29.Delete("HEAT_NO");
			/**************** 循环处理每一列的数据 ***************************************/
			tqmtsb0.Insert();
			tqmts29["REC_CREATE_TIME"] = s.datetime;
			tqmts29["REC_CREATOR"] = "qc";
			tqmts29["HEAT_NO"] = tqmtsb0["HEAT_NO"];
			tqmts29["PONO"] = tqmtsb0["PONO"];
			tqmts29["ST_NO"] = tqmtsb0["ST_NO"];
			for (int kkk = 0; kkk < bcls_rec_qc.Tables[0].Columns.get_Count(); kkk++)
			{
				CString col_name = bcls_rec_qc.Tables[0].Columns[kkk].get_ColumnName();
				if (bcls_rec_qc.Tables[0].Columns[kkk].get_ColumnName().ToUpper().Find("ELM_") >= 0)
				{
					tqmts29["ELM_ACT"] = bcls_rec_qc.Tables[0].Rows[iii][kkk].ToDecimal();
					tqmts29["ELM_ACT"] = tqmts29["ELM_ACT"];
					if (tqmts29["ELM_ACT"].ToDecimal() >0)
					{
						tqmts29["ELM_CODE"] = bcls_rec_qc.Tables[0].Columns[kkk].get_ColumnName().ToUpper().Substring(4,3);
						tqmts29["ELM_NAME"] = map_code[tqmts29["ELM_CODE"].ToString()];
						tqmts29.TrimOrBlank();//如果有Null
						tqmts29["ELM_OK"] = "0";
						tqmts29.Insert();
					}

				}

			}
			
			/************** 处理成功，更新tqmtsb0的状态等 ***********/
			sqlstr = "UPDATE PM_SAMPLE  SET AGGREGATENAME = '1' WHERE HEATNUMBER = @heat_no ";
			cmd_upd.SetCommandText(sqlstr);
			cmd_upd.Parameters.Set("heat_no", tqmtsb0["HEAT_NO"]);
			cmd_upd.ExecuteNonQuery();

			/************** 单记录提交 ************************/
			CTransactionManager::Commit(0);
			CTransactionManager::Begin(0, 0);
			continue;
		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
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
