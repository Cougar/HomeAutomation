<?php
require(dirname(__FILE__)."/config.php"); 

echo('
<?xml version="1.0" encoding="utf-8"?>
<wsdl:definitions xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/" 
				  xmlns:tns="http://schemas.microsoft.com/exchange/services/2006/messages" 
				  xmlns:s="http://www.w3.org/2001/XMLSchema" 
				  targetNamespace="http://schemas.microsoft.com/exchange/services/2006/messages"
				  xmlns:wsdl="http://schemas.xmlsoap.org/wsdl/"
				  xmlns:t="http://schemas.microsoft.com/exchange/services/2006/types">
	<wsdl:types>
		<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
			<xs:import namespace="http://schemas.microsoft.com/exchange/services/2006/messages" schemaLocation="messages.xsd"/>
		</xs:schema>
	</wsdl:types>
	<wsdl:message name="ConvertIdSoapIn">
		<wsdl:part name="request" element="tns:ConvertId"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>

	</wsdl:message>
	<wsdl:message name="ConvertIdSoapOut">
		<wsdl:part name="ConvertIdResult" element="tns:ConvertIdResponse"/>
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
	<wsdl:message name="GetFolderSoapIn">
		<wsdl:part name="request" element="tns:GetFolder" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>

		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
	</wsdl:message>
	<wsdl:message name="GetFolderSoapOut">
		<wsdl:part name="GetFolderResult" element="tns:GetFolderResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
	<wsdl:message name="CreateFolderSoapIn">
		<wsdl:part name="request" element="tns:CreateFolder" />

		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
	<wsdl:message name="CreateFolderSoapOut">
		<wsdl:part name="CreateFolderResult" element="tns:CreateFolderResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>

	<wsdl:message name="CreateManagedFolderSoapIn">
		<wsdl:part name="request" element="tns:CreateManagedFolder" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
	<wsdl:message name="CreateManagedFolderSoapOut">
		<wsdl:part name="CreateManagedFolderResult" element="tns:CreateManagedFolderResponse" />

		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
	<wsdl:message name="DeleteFolderSoapIn">
		<wsdl:part name="request" element="tns:DeleteFolder" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>

	<wsdl:message name="DeleteFolderSoapOut">
		<wsdl:part name="DeleteFolderResult" element="tns:DeleteFolderResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
	<wsdl:message name="UpdateFolderSoapIn">
		<wsdl:part name="request" element="tns:UpdateFolder" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>

 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
	<wsdl:message name="UpdateFolderSoapOut">
		<wsdl:part name="UpdateFolderResult" element="tns:UpdateFolderResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
	<wsdl:message name="MoveFolderSoapIn">
		<wsdl:part name="request" element="tns:MoveFolder" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>

		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
	<wsdl:message name="MoveFolderSoapOut">
		<wsdl:part name="MoveFolderResult" element="tns:MoveFolderResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
	<wsdl:message name="CopyFolderSoapIn">

		<wsdl:part name="request" element="tns:CopyFolder" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
	<wsdl:message name="CopyFolderSoapOut">
		<wsdl:part name="CopyFolderResult" element="tns:CopyFolderResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>

	</wsdl:message>
	<wsdl:message name="FindFolderSoapIn">
		<wsdl:part name="request" element="tns:FindFolder"/>
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
	<wsdl:message name="FindFolderSoapOut">

		<wsdl:part name="FindFolderResult" element="tns:FindFolderResponse"/>
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
	<wsdl:message name="GetItemSoapIn">
		<wsdl:part name="request" element="tns:GetItem" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>

  </wsdl:message>
	<wsdl:message name="GetItemSoapOut">
		<wsdl:part name="GetItemResult" element="tns:GetItemResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
	<wsdl:message name="CreateItemSoapIn">
		<wsdl:part name="request" element="tns:CreateItem" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>

		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
	<wsdl:message name="CreateItemSoapOut">
		<wsdl:part name="CreateItemResult" element="tns:CreateItemResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
	<wsdl:message name="DeleteItemSoapIn">
		<wsdl:part name="request" element="tns:DeleteItem" />

		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
	<wsdl:message name="DeleteItemSoapOut">
		<wsdl:part name="DeleteItemResult" element="tns:DeleteItemResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>

	<wsdl:message name="UpdateItemSoapIn">
		<wsdl:part name="request" element="tns:UpdateItem" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
	<wsdl:message name="UpdateItemSoapOut">
		<wsdl:part name="UpdateItemResult" element="tns:UpdateItemResponse" />

		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
	<wsdl:message name="SendItemSoapIn">
		<wsdl:part name="request" element="tns:SendItem" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>

	<wsdl:message name="SendItemSoapOut">
		<wsdl:part name="SendItemResult" element="tns:SendItemResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
	<wsdl:message name="MoveItemSoapIn">
		<wsdl:part name="request" element="tns:MoveItem" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>

 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
	<wsdl:message name="MoveItemSoapOut">
		<wsdl:part name="MoveItemResult" element="tns:MoveItemResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
	<wsdl:message name="CopyItemSoapIn">
		<wsdl:part name="request" element="tns:CopyItem" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>

		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
	<wsdl:message name="CopyItemSoapOut">
		<wsdl:part name="CopyItemResult" element="tns:CopyItemResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
	<wsdl:message name="CreateAttachmentSoapIn">

		<wsdl:part name="request" element="tns:CreateAttachment" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
	<wsdl:message name="CreateAttachmentSoapOut">
		<wsdl:part name="CreateAttachmentResult" element="tns:CreateAttachmentResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>

	</wsdl:message>
	<wsdl:message name="DeleteAttachmentSoapIn">
		<wsdl:part name="request" element="tns:DeleteAttachment" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
	<wsdl:message name="DeleteAttachmentSoapOut">

		<wsdl:part name="DeleteAttachmentResult" element="tns:DeleteAttachmentResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
	<wsdl:message name="GetAttachmentSoapIn">
		<wsdl:part name="request" element="tns:GetAttachment" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>

  </wsdl:message>
	<wsdl:message name="GetAttachmentSoapOut">
		<wsdl:part name="GetAttachmentResult" element="tns:GetAttachmentResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
	<wsdl:message name="ResolveNamesSoapIn">
		<wsdl:part name="request" element="tns:ResolveNames" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>

		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
	<wsdl:message name="ResolveNamesSoapOut">
		<wsdl:part name="ResolveNamesResult" element="tns:ResolveNamesResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
	<wsdl:message name="ExpandDLSoapIn">
		<wsdl:part name="request" element="tns:ExpandDL" />

		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
	<wsdl:message name="ExpandDLSoapOut">
		<wsdl:part name="ExpandDLResult" element="tns:ExpandDLResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>

	<wsdl:message name="FindItemSoapIn">
		<wsdl:part name="request" element="tns:FindItem"/>
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
	<wsdl:message name="FindItemSoapOut">
		<wsdl:part name="FindItemResult" element="tns:FindItemResponse"/>

		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>

	<wsdl:message name="SubscribeSoapIn">
		<wsdl:part name="request" element="tns:Subscribe" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>

  </wsdl:message>
	<wsdl:message name="SubscribeSoapOut">
		<wsdl:part name="SubscribeResult" element="tns:SubscribeResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
	<wsdl:message name="UnsubscribeSoapIn">
		<wsdl:part name="request" element="tns:Unsubscribe" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>

		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
	<wsdl:message name="UnsubscribeSoapOut">
		<wsdl:part name="UnsubscribeResult" element="tns:UnsubscribeResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
	<wsdl:message name="GetEventsSoapIn">
		<wsdl:part name="request" element="tns:GetEvents" />

		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
	<wsdl:message name="GetEventsSoapOut">
		<wsdl:part name="GetEventsResult" element="tns:GetEventsResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>

	<wsdl:message name="SyncFolderHierarchySoapIn">
		<wsdl:part name="request" element="tns:SyncFolderHierarchy" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
 		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
	<wsdl:message name="SyncFolderHierarchySoapOut">
		<wsdl:part name="SyncFolderHierarchyResult" element="tns:SyncFolderHierarchyResponse" />

		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
	<wsdl:message name="SyncFolderItemsSoapIn">
		<wsdl:part name="request" element="tns:SyncFolderItems" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>

	<wsdl:message name="SyncFolderItemsSoapOut">
		<wsdl:part name="SyncFolderItemsResult" element="tns:SyncFolderItemsResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
	</wsdl:message>
  
  <wsdl:message name="GetDelegateSoapIn">
		<wsdl:part name="request" element="tns:GetDelegate" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>

		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
  <wsdl:message name="GetDelegateSoapOut">
		<wsdl:part name="GetDelegateResult" element="tns:GetDelegateResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
  </wsdl:message>
  <wsdl:message name="AddDelegateSoapIn">
		<wsdl:part name="request" element="tns:AddDelegate" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>

		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
  <wsdl:message name="AddDelegateSoapOut">
		<wsdl:part name="AddDelegateResult" element="tns:AddDelegateResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
  </wsdl:message>
  <wsdl:message name="RemoveDelegateSoapIn">

		<wsdl:part name="request" element="tns:RemoveDelegate" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
  <wsdl:message name="RemoveDelegateSoapOut">
		<wsdl:part name="RemoveDelegateResult" element="tns:RemoveDelegateResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>

  </wsdl:message>
  <wsdl:message name="UpdateDelegateSoapIn">
		<wsdl:part name="request" element="tns:UpdateDelegate" />
		<wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
		<wsdl:part name="S2SAuth" element="t:SerializedSecurityContext"/>
		<wsdl:part name="MailboxCulture" element="t:MailboxCulture"/>
		<wsdl:part name="RequestVersion" element="t:RequestServerVersion"/>
  </wsdl:message>
  <wsdl:message name="UpdateDelegateSoapOut">

		<wsdl:part name="UpdateDelegateResult" element="tns:UpdateDelegateResponse" />
		<wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
  </wsdl:message>

   <!-- Availability Service messages -->

  <wsdl:message name="GetUserAvailabilitySoapIn">
    <wsdl:part name="GetUserAvailabilityRequest" element="tns:GetUserAvailabilityRequest" />
    <wsdl:part name="SerializedSecurityContext" element="t:SerializedSecurityContext" />
    <wsdl:part name="ProxyRequestTypeHeader" element="t:ProxyRequestTypeHeader" />

    <wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
  </wsdl:message>
  <wsdl:message name="GetUserAvailabilitySoapOut">
    <wsdl:part name="GetUserAvailabilityResult" element="tns:GetUserAvailabilityResponse" />
    <wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
  </wsdl:message>
  <wsdl:message name="GetUserOofSettingsSoapIn">
    <wsdl:part name="GetUserOofSettingsRequest" element="tns:GetUserOofSettingsRequest" />
    <wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>

  </wsdl:message>
  <wsdl:message name="GetUserOofSettingsSoapOut">
    <wsdl:part name="GetUserOofSettingsResult" element="tns:GetUserOofSettingsResponse" />
    <wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
  </wsdl:message>
  <wsdl:message name="SetUserOofSettingsSoapIn">
    <wsdl:part name="SetUserOofSettingsRequest" element="tns:SetUserOofSettingsRequest" />
    <wsdl:part name="Impersonation" element="t:ExchangeImpersonation"/>
  </wsdl:message>

  <wsdl:message name="SetUserOofSettingsSoapOut">
    <wsdl:part name="SetUserOofSettingsResult" element="tns:SetUserOofSettingsResponse" />
    <wsdl:part name="ServerVersion" element="t:ServerVersionInfo"/>
  </wsdl:message>
  
	<wsdl:portType name="ExchangeServicePortType">
		<wsdl:operation name="ResolveNames">
			<wsdl:input message="tns:ResolveNamesSoapIn" />
			<wsdl:output message="tns:ResolveNamesSoapOut" />
		</wsdl:operation>

		<wsdl:operation name="ExpandDL">
			<wsdl:input message="tns:ExpandDLSoapIn" />
			<wsdl:output message="tns:ExpandDLSoapOut" />
		</wsdl:operation>
		<wsdl:operation name="FindFolder">
			<wsdl:input message="tns:FindFolderSoapIn"/>
			<wsdl:output message="tns:FindFolderSoapOut"/>
		</wsdl:operation>
		<wsdl:operation name="FindItem">

			<wsdl:input message="tns:FindItemSoapIn"/>
			<wsdl:output message="tns:FindItemSoapOut"/>
		</wsdl:operation>
		<wsdl:operation name="GetFolder">
			<wsdl:input message="tns:GetFolderSoapIn" />
			<wsdl:output message="tns:GetFolderSoapOut" />
		</wsdl:operation>
		<wsdl:operation name="ConvertId">
			<wsdl:input message="tns:ConvertIdSoapIn"/>

			<wsdl:output message="tns:ConvertIdSoapOut"/>
		</wsdl:operation>
		<wsdl:operation name="CreateFolder">
			<wsdl:input message="tns:CreateFolderSoapIn" />
			<wsdl:output message="tns:CreateFolderSoapOut" />
		</wsdl:operation>
		<wsdl:operation name="DeleteFolder">
			<wsdl:input message="tns:DeleteFolderSoapIn" />
			<wsdl:output message="tns:DeleteFolderSoapOut" />

		</wsdl:operation>
		<wsdl:operation name="UpdateFolder">
			<wsdl:input message="tns:UpdateFolderSoapIn" />
			<wsdl:output message="tns:UpdateFolderSoapOut" />
		</wsdl:operation>
		<wsdl:operation name="MoveFolder">
			<wsdl:input message="tns:MoveFolderSoapIn" />
			<wsdl:output message="tns:MoveFolderSoapOut" />
		</wsdl:operation>

		<wsdl:operation name="CopyFolder">
			<wsdl:input message="tns:CopyFolderSoapIn" />
			<wsdl:output message="tns:CopyFolderSoapOut" />
		</wsdl:operation>
		<wsdl:operation name="Subscribe">
			<wsdl:input message="tns:SubscribeSoapIn" />
			<wsdl:output message="tns:SubscribeSoapOut" />
		</wsdl:operation>
		<wsdl:operation name="Unsubscribe">

			<wsdl:input message="tns:UnsubscribeSoapIn" />
			<wsdl:output message="tns:UnsubscribeSoapOut" />
		</wsdl:operation>
		<wsdl:operation name="GetEvents">
			<wsdl:input message="tns:GetEventsSoapIn" />
			<wsdl:output message="tns:GetEventsSoapOut" />
		</wsdl:operation>
		<wsdl:operation name="SyncFolderHierarchy">
			<wsdl:input message="tns:SyncFolderHierarchySoapIn" />

			<wsdl:output message="tns:SyncFolderHierarchySoapOut" />
		</wsdl:operation>
		<wsdl:operation name="SyncFolderItems">
			<wsdl:input message="tns:SyncFolderItemsSoapIn" />
			<wsdl:output message="tns:SyncFolderItemsSoapOut" />
		</wsdl:operation>

		<wsdl:operation name="CreateManagedFolder">
			<wsdl:input message="tns:CreateManagedFolderSoapIn" />

			<wsdl:output message="tns:CreateManagedFolderSoapOut" />
		</wsdl:operation>

		<wsdl:operation name="GetItem">
			<wsdl:input message="tns:GetItemSoapIn" />
			<wsdl:output message="tns:GetItemSoapOut" />
		</wsdl:operation>
		<wsdl:operation name="CreateItem">
			<wsdl:input message="tns:CreateItemSoapIn" />

			<wsdl:output message="tns:CreateItemSoapOut" />
		</wsdl:operation>
		<wsdl:operation name="DeleteItem">
			<wsdl:input message="tns:DeleteItemSoapIn" />
			<wsdl:output message="tns:DeleteItemSoapOut" />
		</wsdl:operation>
		<wsdl:operation name="UpdateItem">
			<wsdl:input message="tns:UpdateItemSoapIn" />
			<wsdl:output message="tns:UpdateItemSoapOut" />

		</wsdl:operation>
		<wsdl:operation name="SendItem">
			<wsdl:input message="tns:SendItemSoapIn" />
			<wsdl:output message="tns:SendItemSoapOut" />
		</wsdl:operation>
		<wsdl:operation name="MoveItem">
			<wsdl:input message="tns:MoveItemSoapIn" />
			<wsdl:output message="tns:MoveItemSoapOut" />
		</wsdl:operation>

		<wsdl:operation name="CopyItem">
			<wsdl:input message="tns:CopyItemSoapIn" />
			<wsdl:output message="tns:CopyItemSoapOut" />
		</wsdl:operation>
		<wsdl:operation name="CreateAttachment">
			<wsdl:input message="tns:CreateAttachmentSoapIn" />
			<wsdl:output message="tns:CreateAttachmentSoapOut" />
		</wsdl:operation>
		<wsdl:operation name="DeleteAttachment">

			<wsdl:input message="tns:DeleteAttachmentSoapIn" />
			<wsdl:output message="tns:DeleteAttachmentSoapOut" />
		</wsdl:operation>
		<wsdl:operation name="GetAttachment">
			<wsdl:input message="tns:GetAttachmentSoapIn" />
			<wsdl:output message="tns:GetAttachmentSoapOut" />
		</wsdl:operation>
		<wsdl:operation name="GetDelegate">
			<wsdl:input message="tns:GetDelegateSoapIn" />

			<wsdl:output message="tns:GetDelegateSoapOut" />
		</wsdl:operation>
		<wsdl:operation name="AddDelegate">
			<wsdl:input message="tns:AddDelegateSoapIn" />
			<wsdl:output message="tns:AddDelegateSoapOut" />
		</wsdl:operation>
		<wsdl:operation name="RemoveDelegate">
			<wsdl:input message="tns:RemoveDelegateSoapIn" />
			<wsdl:output message="tns:RemoveDelegateSoapOut" />

		</wsdl:operation>
		<wsdl:operation name="UpdateDelegate">
			<wsdl:input message="tns:UpdateDelegateSoapIn" />
			<wsdl:output message="tns:UpdateDelegateSoapOut" />
		</wsdl:operation>

    <!-- Availability Service Operations -->

    <!-- GetUserAvailability -->
    <wsdl:operation name="GetUserAvailability">

      <wsdl:input message="tns:GetUserAvailabilitySoapIn" />
      <wsdl:output message="tns:GetUserAvailabilitySoapOut" />
    </wsdl:operation>

    <!-- GetUserOofSettings -->
    <wsdl:operation name="GetUserOofSettings">
      <wsdl:input message="tns:GetUserOofSettingsSoapIn" />
      <wsdl:output message="tns:GetUserOofSettingsSoapOut" />
    </wsdl:operation>

    <!-- SetUserOofSettings -->
    <wsdl:operation name="SetUserOofSettings">
      <wsdl:input message="tns:SetUserOofSettingsSoapIn" />
      <wsdl:output message="tns:SetUserOofSettingsSoapOut" />
    </wsdl:operation>  
  </wsdl:portType>

	<wsdl:binding name="ExchangeServiceBinding" type="tns:ExchangeServicePortType">
		<wsdl:documentation>

			<wsi:Claim conformsTo="http://ws-i.org/profiles/basic/1.0" xmlns:wsi="http://ws-i.org/schemas/conformanceClaim/" />
		</wsdl:documentation>
		<soap:binding transport="http://schemas.xmlsoap.org/soap/http" style="document" />
		<wsdl:operation name="ResolveNames">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/ResolveNames" />
			<wsdl:input>
				<soap:body parts="request" use="literal"/>
				<soap:header message="tns:ResolveNamesSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:ResolveNamesSoapIn" part="S2SAuth" use="literal"/>

        <soap:header message="tns:ResolveNamesSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:ResolveNamesSoapIn" part="RequestVersion" use="literal"/>
      </wsdl:input>
			<wsdl:output>
				<soap:body parts="ResolveNamesResult" use="literal" />
				<soap:header message="tns:ResolveNamesSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>
		<wsdl:operation name="ExpandDL">

			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/ExpandDL" />
			<wsdl:input>
				<soap:header message="tns:ExpandDLSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:ExpandDLSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:ExpandDLSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:ExpandDLSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>

				<soap:body parts="ExpandDLResult" use="literal" />
				<soap:header message="tns:ExpandDLSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>

		<wsdl:operation name="FindFolder">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/FindFolder"/>
			<wsdl:input>
				<soap:header message="tns:FindFolderSoapIn" part="Impersonation" use="literal"/>

				<soap:header message="tns:FindFolderSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:FindFolderSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:FindFolderSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>
				<soap:body parts="FindFolderResult" use="literal" />
				<soap:header message="tns:FindFolderSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>

		</wsdl:operation>
		<wsdl:operation name="FindItem">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/FindItem"/>
			<wsdl:input>
				<soap:header message="tns:FindItemSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:FindItemSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:FindItemSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:FindItemSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />

			</wsdl:input>
			<wsdl:output>
				<soap:body parts="FindItemResult" use="literal" />
				<soap:header message="tns:FindItemSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>
		<wsdl:operation name="GetFolder">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/GetFolder" />
			<wsdl:input>

				<soap:header message="tns:GetFolderSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:GetFolderSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:GetFolderSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:GetFolderSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>
				<soap:body parts="GetFolderResult" use="literal" />
				<soap:header message="tns:GetFolderSoapOut" part="ServerVersion" use="literal"/>

			</wsdl:output>
		</wsdl:operation>
		<wsdl:operation name="ConvertId">
      <soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/ConvertId" />
			<wsdl:input>
        <soap:header message="tns:ConvertIdSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>

				<soap:body parts="ConvertIdResult" use="literal" />
				<soap:header message="tns:ConvertIdSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>
		<wsdl:operation name="CreateFolder">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/CreateFolder" />
			<wsdl:input>
				<soap:header message="tns:CreateFolderSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:CreateFolderSoapIn" part="S2SAuth" use="literal"/>

				<soap:header message="tns:CreateFolderSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:CreateFolderSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>
				<soap:body parts="CreateFolderResult" use="literal" />
				<soap:header message="tns:CreateFolderSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>

		<wsdl:operation name="DeleteFolder">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/DeleteFolder" />
			<wsdl:input>
				<soap:header message="tns:DeleteFolderSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:DeleteFolderSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:DeleteFolderSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:DeleteFolderSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>

			<wsdl:output>
				<soap:body parts="DeleteFolderResult" use="literal" />
				<soap:header message="tns:DeleteFolderSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>
		<wsdl:operation name="UpdateFolder">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/UpdateFolder" />
			<wsdl:input>
				<soap:header message="tns:UpdateFolderSoapIn" part="Impersonation" use="literal"/>

				<soap:header message="tns:UpdateFolderSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:UpdateFolderSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:UpdateFolderSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>
				<soap:body parts="UpdateFolderResult" use="literal" />
				<soap:header message="tns:UpdateFolderSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>

		</wsdl:operation>
		<wsdl:operation name="MoveFolder">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/MoveFolder" />
			<wsdl:input>
				<soap:header message="tns:MoveFolderSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:MoveFolderSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:MoveFolderSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:MoveFolderSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />

			</wsdl:input>
			<wsdl:output>
				<soap:body parts="MoveFolderResult" use="literal" />
				<soap:header message="tns:MoveFolderSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>
		<wsdl:operation name="CopyFolder">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/CopyFolder" />
			<wsdl:input>

				<soap:header message="tns:CopyFolderSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:CopyFolderSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:CopyFolderSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:CopyFolderSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>
				<soap:body parts="CopyFolderResult" use="literal" />
				<soap:header message="tns:CopyFolderSoapOut" part="ServerVersion" use="literal"/>

			</wsdl:output>
		</wsdl:operation>
		<wsdl:operation name="Subscribe">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/Subscribe" />
			<wsdl:input>
				<soap:header message="tns:SubscribeSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:SubscribeSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:SubscribeSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:SubscribeSoapIn" part="RequestVersion" use="literal"/>

        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>
				<soap:body parts="SubscribeResult" use="literal" />
				<soap:header message="tns:SubscribeSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>
		<wsdl:operation name="Unsubscribe">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/Unsubscribe" />

			<wsdl:input>
				<soap:header message="tns:UnsubscribeSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:UnsubscribeSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:UnsubscribeSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:UnsubscribeSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>
				<soap:body parts="UnsubscribeResult" use="literal" />

				<soap:header message="tns:UnsubscribeSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>
		<wsdl:operation name="GetEvents">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/GetEvents" />
			<wsdl:input>
				<soap:header message="tns:GetEventsSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:GetEventsSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:GetEventsSoapIn" part="MailboxCulture" use="literal"/>

        <soap:header message="tns:GetEventsSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>
				<soap:body parts="GetEventsResult" use="literal" />
				<soap:header message="tns:GetEventsSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>
		<wsdl:operation name="SyncFolderHierarchy">

			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/SyncFolderHierarchy" />
			<wsdl:input>
				<soap:header message="tns:SyncFolderHierarchySoapIn" part="Impersonation" use="literal"></soap:header>
				<soap:header message="tns:SyncFolderHierarchySoapIn" part="S2SAuth" use="literal"/>
        <soap:header message="tns:SyncFolderHierarchySoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:SyncFolderHierarchySoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>

				<soap:body parts="SyncFolderHierarchyResult" use="literal" />
				<soap:header message="tns:SyncFolderHierarchySoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>
		<wsdl:operation name="SyncFolderItems">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/SyncFolderItems" />
			<wsdl:input>
				<soap:header message="tns:SyncFolderItemsSoapIn" part="Impersonation" use="literal"></soap:header>
				<soap:header message="tns:SyncFolderItemsSoapIn" part="S2SAuth" use="literal"/>

        <soap:header message="tns:SyncFolderItemsSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:SyncFolderItemsSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>
				<soap:body parts="SyncFolderItemsResult" use="literal" />
				<soap:header message="tns:SyncFolderItemsSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>

		<wsdl:operation name="GetItem">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/GetItem" />
			<wsdl:input>
				<soap:header message="tns:GetItemSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:GetItemSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:GetItemSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:GetItemSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />

			</wsdl:input>
			<wsdl:output>
				<soap:body parts="GetItemResult" use="literal" />
				<soap:header message="tns:GetItemSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>
		<wsdl:operation name="CreateItem">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/CreateItem" />
			<wsdl:input>

				<soap:header message="tns:CreateItemSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:CreateItemSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:CreateItemSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:CreateItemSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>
				<soap:body parts="CreateItemResult" use="literal" />
				<soap:header message="tns:CreateItemSoapOut" part="ServerVersion" use="literal"/>

			</wsdl:output>
		</wsdl:operation>
		<wsdl:operation name="DeleteItem">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/DeleteItem" />
			<wsdl:input>
				<soap:header message="tns:DeleteItemSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:DeleteItemSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:DeleteItemSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:DeleteItemSoapIn" part="RequestVersion" use="literal"/>

        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>
				<soap:body parts="DeleteItemResult" use="literal" />
				<soap:header message="tns:DeleteItemSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>
		<wsdl:operation name="UpdateItem">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/UpdateItem" />

			<wsdl:input>
				<soap:header message="tns:UpdateItemSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:UpdateItemSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:UpdateItemSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:UpdateItemSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>
				<soap:body parts="UpdateItemResult" use="literal" />

				<soap:header message="tns:UpdateItemSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>
		<wsdl:operation name="SendItem">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/SendItem" />
			<wsdl:input>
				<soap:header message="tns:SendItemSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:SendItemSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:SendItemSoapIn" part="MailboxCulture" use="literal"/>

        <soap:header message="tns:SendItemSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>
				<soap:body parts="SendItemResult" use="literal" />
				<soap:header message="tns:SendItemSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>
		<wsdl:operation name="MoveItem">

			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/MoveItem" />
			<wsdl:input>
				<soap:header message="tns:MoveItemSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:MoveItemSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:MoveItemSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:MoveItemSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>

				<soap:body parts="MoveItemResult" use="literal" />
				<soap:header message="tns:MoveItemSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>
		<wsdl:operation name="CopyItem">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/CopyItem" />
			<wsdl:input>
				<soap:header message="tns:CopyItemSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:CopyItemSoapIn" part="S2SAuth" use="literal"/>

				<soap:header message="tns:CopyItemSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:CopyItemSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>
				<soap:body parts="CopyItemResult" use="literal" />
				<soap:header message="tns:CopyItemSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>

		<wsdl:operation name="CreateAttachment">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/CreateAttachment" />
			<wsdl:input>
				<soap:header message="tns:CreateAttachmentSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:CreateAttachmentSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:CreateAttachmentSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:CreateAttachmentSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>

			<wsdl:output>
				<soap:body parts="CreateAttachmentResult" use="literal" />
				<soap:header message="tns:CreateAttachmentSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>
		<wsdl:operation name="DeleteAttachment">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/DeleteAttachment" />
			<wsdl:input>
				<soap:header message="tns:DeleteAttachmentSoapIn" part="Impersonation" use="literal"/>

				<soap:header message="tns:DeleteAttachmentSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:DeleteAttachmentSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:DeleteAttachmentSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>
				<soap:body parts="DeleteAttachmentResult" use="literal" />
				<soap:header message="tns:DeleteAttachmentSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>

		</wsdl:operation>
		<wsdl:operation name="GetAttachment">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/GetAttachment" />
			<wsdl:input>
				<soap:header message="tns:GetAttachmentSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:GetAttachmentSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:GetAttachmentSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:GetAttachmentSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />

			</wsdl:input>
			<wsdl:output>
				<soap:body parts="GetAttachmentResult" use="literal" />
				<soap:header message="tns:GetAttachmentSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>

		<wsdl:operation name="CreateManagedFolder">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/CreateManagedFolder" />

			<wsdl:input>
				<soap:header message="tns:CreateManagedFolderSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:CreateManagedFolderSoapIn" part="S2SAuth" use="literal"/>
        <soap:header message="tns:CreateManagedFolderSoapIn" part="MailboxCulture" use="literal"/>
        <soap:header message="tns:CreateManagedFolderSoapIn" part="RequestVersion" use="literal"/>
        <soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>
				<soap:body parts="CreateManagedFolderResult" use="literal" />

				<soap:header message="tns:CreateManagedFolderSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>

		<wsdl:operation name="GetDelegate">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/GetDelegate" />
			<wsdl:input>
				<soap:header message="tns:GetDelegateSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:GetDelegateSoapIn" part="S2SAuth" use="literal"/>

				<soap:header message="tns:GetDelegateSoapIn" part="MailboxCulture" use="literal"/>
				<soap:header message="tns:GetDelegateSoapIn" part="RequestVersion" use="literal"/>
				<soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>
				<soap:body parts="GetDelegateResult" use="literal" />
				<soap:header message="tns:GetDelegateSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>

		<wsdl:operation name="AddDelegate">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/AddDelegate" />
			<wsdl:input>
				<soap:header message="tns:AddDelegateSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:AddDelegateSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:AddDelegateSoapIn" part="MailboxCulture" use="literal"/>
				<soap:header message="tns:AddDelegateSoapIn" part="RequestVersion" use="literal"/>
				<soap:body parts="request" use="literal" />

			</wsdl:input>
			<wsdl:output>
				<soap:body parts="AddDelegateResult" use="literal" />
				<soap:header message="tns:AddDelegateSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>

		<wsdl:operation name="RemoveDelegate">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/RemoveDelegate" />

			<wsdl:input>
				<soap:header message="tns:RemoveDelegateSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:RemoveDelegateSoapIn" part="S2SAuth" use="literal"/>
				<soap:header message="tns:RemoveDelegateSoapIn" part="MailboxCulture" use="literal"/>
				<soap:header message="tns:RemoveDelegateSoapIn" part="RequestVersion" use="literal"/>
				<soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>
				<soap:body parts="RemoveDelegateResult" use="literal" />

				<soap:header message="tns:RemoveDelegateSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>

		<wsdl:operation name="UpdateDelegate">
			<soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/UpdateDelegate" />
			<wsdl:input>
				<soap:header message="tns:UpdateDelegateSoapIn" part="Impersonation" use="literal"/>
				<soap:header message="tns:UpdateDelegateSoapIn" part="S2SAuth" use="literal"/>

				<soap:header message="tns:UpdateDelegateSoapIn" part="MailboxCulture" use="literal"/>
				<soap:header message="tns:UpdateDelegateSoapIn" part="RequestVersion" use="literal"/>
				<soap:body parts="request" use="literal" />
			</wsdl:input>
			<wsdl:output>
				<soap:body parts="UpdateDelegateResult" use="literal" />
				<soap:header message="tns:UpdateDelegateSoapOut" part="ServerVersion" use="literal"/>
			</wsdl:output>
		</wsdl:operation>

		
		<!-- Availability Service -->

    <!-- GetUserAvailability -->
    <wsdl:operation name="GetUserAvailability">
      <soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/GetUserAvailability" />
      <wsdl:input>
        <soap:body parts="GetUserAvailabilityRequest" use="literal" />
        <soap:header message="tns:GetUserAvailabilitySoapIn" part="SerializedSecurityContext" use="literal" />
        <soap:header message="tns:GetUserAvailabilitySoapIn" part="ProxyRequestTypeHeader" use="literal" />

      </wsdl:input>
      <wsdl:output>
        <soap:body parts="GetUserAvailabilityResult" use="literal" />
        <soap:header message="tns:GetUserAvailabilitySoapOut" part="ServerVersion" use="literal"/>
      </wsdl:output>
    </wsdl:operation>

    <!-- GetUserOofSettings -->
    <wsdl:operation name="GetUserOofSettings">

      <soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/GetUserOofSettings" />
      <wsdl:input>
        <soap:body parts="GetUserOofSettingsRequest" use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap:body parts="GetUserOofSettingsResult" use="literal" />
        <soap:header message="tns:GetUserOofSettingsSoapOut" part="ServerVersion" use="literal"/>
      </wsdl:output>
    </wsdl:operation>

    <!-- SetUserOofSettings -->
    <wsdl:operation name="SetUserOofSettings">
      <soap:operation soapAction="http://schemas.microsoft.com/exchange/services/2006/messages/SetUserOofSettings" />
      <wsdl:input>
        <soap:body parts="SetUserOofSettingsRequest" use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap:body parts="SetUserOofSettingsResult" use="literal" />

        <soap:header message="tns:SetUserOofSettingsSoapOut" part="ServerVersion" use="literal"/>
      </wsdl:output>
    </wsdl:operation>    
  </wsdl:binding>
   <wsdl:service name="ExchangeServices">
     <wsdl:port name="ExchangeServicePort" binding="tns:ExchangeServiceBinding">
       <soap:address location="');

echo($exchangeurl); 

echo('"/>
     </wsdl:port>
  </wsdl:service> 
</wsdl:definitions>
');

?>
