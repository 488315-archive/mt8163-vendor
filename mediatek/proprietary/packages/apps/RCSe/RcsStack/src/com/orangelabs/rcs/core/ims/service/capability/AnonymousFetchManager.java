/*******************************************************************************
 * Software Name : RCS IMS Stack
 *
 * Copyright (C) 2010 France Telecom S.A.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/

package com.orangelabs.rcs.core.ims.service.capability;

import java.io.ByteArrayInputStream;
import java.util.List;
import java.util.Vector;

import org.xml.sax.InputSource;

import com.orangelabs.rcs.core.ims.ImsModule;
import com.orangelabs.rcs.core.ims.network.sip.Multipart;
import com.orangelabs.rcs.core.ims.network.sip.PresenceMultipart;
import com.orangelabs.rcs.core.ims.network.sip.SipUtils;
import com.orangelabs.rcs.core.ims.protocol.sip.SipRequest;
import com.orangelabs.rcs.core.ims.service.ContactInfo;
import com.orangelabs.rcs.core.ims.service.presence.PresenceUtils;
import com.orangelabs.rcs.core.ims.service.presence.pidf.CapabilityDetails;
import com.orangelabs.rcs.core.ims.service.presence.pidf.PidfDocument;
import com.orangelabs.rcs.core.ims.service.presence.pidf.PidfParser;
import com.orangelabs.rcs.core.ims.service.presence.pidf.Tuple;
import com.orangelabs.rcs.core.ims.service.presence.rlmi.ResourceInstance;
import com.orangelabs.rcs.core.ims.service.presence.rlmi.RlmiDocument;
import com.orangelabs.rcs.core.ims.service.presence.rlmi.RlmiParser;
import com.orangelabs.rcs.provider.eab.ContactsManager;
import com.orangelabs.rcs.utils.PhoneUtils;
import com.orangelabs.rcs.utils.logger.Logger;
import java.util.ArrayList;
import javax2.sip.header.SubscriptionStateHeader;

/**
 * Capability discovery manager using anonymous fetch procedure
 * 
 * @author Jean-Marc AUFFRET
 */
public class AnonymousFetchManager implements DiscoveryManager {
       /**
     * IMS module
     */
    private ImsModule imsModule;
    
    /**
     * The logger
     */
    private Logger logger = Logger.getLogger(this.getClass().getName());

    /**
     * Constructor
     * 
     * @param parent IMS module
     */
    public AnonymousFetchManager(ImsModule parent) {
        this.imsModule = parent;
    }
    
    /**
     * Request contact capabilities
     * 
     * @param contact Remote contact
     * @return Returns true if success
     */
    public boolean requestCapabilities(String contact) {
        if (logger.isActivated()) {
            logger.debug("Request capabilities in background for " + contact);
        }
        AnonymousFetchRequestTask task = new AnonymousFetchRequestTask(imsModule, contact);
        task.start();
        return true;
    }
    
    /**
     * Request contact capabilities
     * 
     * @param contact Remote contact
     * @return Returns true if success
     */
    public boolean requestCapabilities(List<String> contact) {
        if (logger.isActivated()) {
            logger.debug("Request capabilities in background for " + contact);
        }
        AnonymousFetchRequestTask task = new AnonymousFetchRequestTask(imsModule, contact);
        task.start();
        return true;
    }
    
    /**
     * Receive a notification
     * 
     * @param notify Received notify
     *//*
    public void receiveNotification(SipRequest notify) {
        if (logger.isActivated()) {
            logger.debug("Anonymous fetch notification received");
        }
        
        
        
        //sample out the data for contacts
        String presenceContent = notify.getContent();
        String boundary = notify.getBoundaryContentType();
        
        
      
PIDF sample   
"<?xml version=\"1.0\" encoding=\"UTF-8\"?><presence xmlns="urn:ietf:params:xml:ns:pidf" xmlns:dm="urn:ietf:params:xml:ns:pidf:data-model" xmlns:rpid="urn:ietf:params:xml:ns:pidf:rpid" xmlns:c="urn:ietf:params:xml:ns:pidf:cipid" xmlns:op="urn:oma:xml:prs:pidf:oma-pres" xmlns:opd="urn:oma:xml:prs:pidf:ext" xmlns:caps="urn:ietf:params:xml:ns:pidf:caps" xmlns:g="urn:summit:ns:pidf:timezone" xmlns:pdm="urn:ietf:params:xml:ns:pidf:data-model" entity="sip:14252339105@msg.pc.t-mobile.com"><tuple id="SessModeMessa"><status><basic>open</basic></status><timestamp>2015-05-29T08:32:23Z</timestamp><contact priority="0.000">sip:14252339105@msg.pc.t-mobile.com</contact><op:service-description><op:service-id>org.openmobilealliance:ChatSession</op:service-id><op:version>2.0</op:version><op:description>Session Mode Messaging</op:description></op:service-description></tuple><tuple id="FileTransfer2"><status><basic>open</basic></status><timestamp>2015-05-29T08:32:23Z</timestamp><contact priority="0.000">sip:14252339105@msg.pc.t-mobile.com</contact><op:service-description><op:service-id>org.openmobilealliance:File-Transfer</op:service-id><op:version>1.0</op:version><op:description>File Transfer</op:description></op:service-description></tuple><tuple id="DiscoveryPres"><status><basic>open</basic></status><timestamp>2015-05-29T08:32:23Z</timestamp><contact priority="0.000">sip:14252339105@msg.pc.t-mobile.com</contact><op:service-description><op:service-id>org.3gpp.urn:urn-7:3gpp-application.ims.iari.rcse.dp</op:service-id><op:version>1.0</op:version><op:description>DiscoveryPresence</op:description></op:service-description></tuple><tuple id="VOLTE26642334"><status><basic>open</basic></status><timestamp>2015-05-29T08:32:23Z</timestamp><contact priority="0.000">sip:14252339105@msg.pc.t-mobile.com</contact><op:service-description><op:service-id>org.3gpp.urn:urn-7:3gpp-service.ims.icsi.mmtel</op:service-id><op:version>1.0</op:version><op:description>IPVideoCall</op:description></op:service-description><caps:servcaps><caps:audio>true</caps:audio><caps:video>false</caps:video><caps:duplex><caps:supported><caps:full/></caps:supported></caps:duplex></caps:servcaps></tuple><tuple id="FtThumbnail26"><status><basic>open</basic></status><timestamp>2015-05-29T08:32:23Z</timestamp><contact priority="0.000">sip:14252339105@msg.pc.t-mobile.com</contact><op:service-description><op:service-id>org.openmobilealliance:File-Transfer-thumb</op:service-id><op:version>2.0</op:version><op:description>FtThumbnail</op:description></op:service-description></tuple></presence>";
       
        PresenceMultipart multi = new PresenceMultipart(presenceContent, boundary);
        ArrayList<String> listPidfXML = new ArrayList<String>() ;
       // String pidfXML = "";
        if (multi.isMultipart()) {
           // Extract resource-lists
            listPidfXML = multi.getPart("application/pidf-diff+xml");
        }
        
        if (logger.isActivated()) {
            logger.debug("list pidfXML data -- > "+listPidfXML);
        }
        
        
        
        for (String pidfXML : listPidfXML) { 
        
        // Parse XML part
             byte[] content = pidfXML.getBytes();
        if (content != null) {
            if (logger.isActivated()) {
                logger.debug("Anonymous fetch notification with PIDF document");
            }
            try {
                InputSource pidfInput = new InputSource(new ByteArrayInputStream(content));
                PidfParser pidfParser = new PidfParser(pidfInput);
                PidfDocument presence = pidfParser.getPresence();
                if (presence != null) {
                    // Extract capabilities
                    Capabilities capabilities =  new Capabilities();
                    int registrationState =  ContactInfo.REGISTRATION_STATUS_UNKNOWN;
                    // We queried via anonymous fetch procedure, so set presence discovery to true
                    //capabilities.setPresenceDiscoverySupport(true);
                    
                    String contact = presence.getEntity();
                    if (logger.isActivated()) {
                        logger.debug("receiveCapNotification :" + contact);
                    } 
                    Vector<Tuple> tuples = presence.getTuplesList();
                    
                    if(tuples.size()>0)
                    {
                      //set the registration as true as the device published some capablity
                        // We queried via anonymous fetch procedure, so set presence discovery to true
                        registrationState =  ContactInfo.REGISTRATION_STATUS_ONLINE;
                    }
                    
                    for(int i=0; i < tuples.size(); i++) {
                        Tuple tuple = (Tuple)tuples.elementAt(i);
                        boolean state = false; 
                        if (tuple.getStatus().getBasic().getValue().equals("open")) {
                            state = true;
                        }
                        String id = tuple.getService().getId();
                        
                        if (id.equalsIgnoreCase(PresenceUtils.FEATURE_RCS2_VIDEO_SHARE)) {
                            capabilities.setVideoSharingSupport(state);
                        } else
                        if (id.equalsIgnoreCase(PresenceUtils.FEATURE_RCS2_VIDEO_SHARE)) {
                            capabilities.setVideoSharingSupport(state);
                        } else
                        if (id.equalsIgnoreCase(PresenceUtils.FEATURE_RCS2_IMAGE_SHARE)) {
                            capabilities.setImageSharingSupport(state);
                        } else
                        if (id.equalsIgnoreCase(PresenceUtils.FEATURE_RCS2_FT)) {
                            //file transfer
                            capabilities.setFileTransferSupport(state);
                        } else
                        if (id.equalsIgnoreCase(PresenceUtils.FEATURE_RCS2_CS_VIDEO)) {
                            capabilities.setCsVideoSupport(state);
                        } 
                        else
                        if (id.equalsIgnoreCase(PresenceUtils.FEATURE_RCS2_IP_VOICE_CALL)) {
                          capabilities.setIPVoiceCallSupport(state);
                        }
                        else
                        if (id.equalsIgnoreCase(PresenceUtils.FEATURE_RCS2_DISCOVERY_VIA_PRESENCE)) {
                                capabilities.setPresenceDiscoverySupport(state);
                        }
                        else
                        if ((id.equalsIgnoreCase(PresenceUtils.FEATURE_RCS2_CHAT))||
                            (id.equalsIgnoreCase(PresenceUtils.FEATURE_RCS2_CHAT_2))
                            ) {
                            //IM capability
                            capabilities.setImSessionSupport(state);
                        }
                    }

                    if (logger.isActivated()) {
                        logger.debug("Set Contact Capability via Presence : "+contact);
                    }
                    
                    // Update capabilities in database
                    ContactsManager.getInstance().setContactCapabilities(contact, capabilities, ContactInfo.RCS_CAPABLE, registrationState);

                    // Notify listener
                    imsModule.getCore().getListener().handleCapabilitiesNotification(contact, capabilities);
                }
            } catch(Exception e) {
                if (logger.isActivated()) {
                    logger.error("Can't parse XML notification", e);
                }
            }        
        } else {
            if (logger.isActivated()) {
                logger.debug("Anonymous fetch notification is empty");
            }
            String contact = PhoneUtils.extractNumberFromUri(SipUtils.getAssertedIdentity(notify));

            // Notify content was empty 
            Capabilities capabilities = new Capabilities();

            // Update capabilities in database
            ContactsManager.getInstance().setContactCapabilities(contact, capabilities, ContactInfo.NO_INFO, ContactInfo.REGISTRATION_STATUS_UNKNOWN);

            // Notify listener
            imsModule.getCore().getListener().handleCapabilitiesNotification(contact, capabilities);
        }
    }    
}
*/
    
    public void receiveNotification(SipRequest notify) {
        if (logger.isActivated()) {
            logger.debug("Anonymous fetch notification received");
        }
        
        //sample out the data for contacts
        String presenceContent = notify.getContent();
        
        if(presenceContent != null)
        {
            try {
                String boundary = notify.getBoundaryContentType();
                     
                PresenceMultipart multi = new PresenceMultipart(presenceContent, boundary);
                if (multi.isMultipart()) {
                   
                    // RLMI PART
                    ArrayList<String> rlmiPartArray = multi.getPart("application/rlmi+xml");
                    if (rlmiPartArray != null) {
                        try {
                            String rlmiPart = rlmiPartArray.get(0);
                            // Parse RLMI part
                            InputSource rlmiInput = new InputSource(new ByteArrayInputStream(rlmiPart.getBytes()));
                            RlmiParser rlmiParser = new RlmiParser(rlmiInput);
                            RlmiDocument rlmiInfo = rlmiParser.getResourceInfo();
                            Vector<ResourceInstance> list = rlmiInfo.getResourceList();
                            for(int i=0; i < list.size(); i++) {
                                ResourceInstance res = (ResourceInstance)list.elementAt(i);
                                String contact = res.getUri();
                                String state = res.getState();
                                String reason = res.getReason();
                                
                                /*
                                 * The <registration-state> element SHALL include either
                                    a) the value active indicating that the Presentity has an active registration with a specific service; or
                                    b) the value terminated indicating that the Presentity does not have an active registration with a specific service. 
                                 * 
                                 */
                                
                                if ((contact != null) && (state != null) && (reason != null)) {
                                    if (state.equalsIgnoreCase("terminated") && reason.equalsIgnoreCase("rejected")) {
                                        // It's a "terminated" event with status "rejected" the contact
                                        // should be removed from the "rcs" list
                                      
                                        if (logger.isActivated()) {
                                            logger.debug("contact : "+contact + " ; rejected the presence sharing request ");
                                    }    

                                        //set the state as offline and reset capabilities 
                                        ContactsManager.getInstance().setContactCapabilities(contact, new Capabilities(), ContactInfo.RCS_REVOKED, ContactInfo.REGISTRATION_STATUS_OFFLINE);
                                        imsModule.getPresenceService().getXdmManager().removeContactFromGrantedList(contact);

                                    }    
                                    else if (state.equalsIgnoreCase("terminated") && reason.equalsIgnoreCase("deactivated")) {
                                        // It's a "terminated" event with status "deactivate" 
                                        //the contacts is a RCS contact but offline
                                      
                                        if (logger.isActivated()) {
                                            logger.debug("contact : "+contact + " ; offline now ");
                                        }
                                        
                                        //set the state as offline and reset capabilities 
                                        ContactsManager.getInstance().setContactCapabilities(contact, new Capabilities(), ContactInfo.RCS_CAPABLE, ContactInfo.REGISTRATION_STATUS_OFFLINE);
                                        
                                        //ImsModule.getPresenceService().getXdmManager().removeContactFromGrantedList(contact);
                                    }    
                                    
                                    // Notify listener
                                    //getImsModule().getCore().getListener().handlePresenceSharingNotification(contact, state, reason);
                                }
                            }
                        } catch(Exception e) {
                            if (logger.isActivated()) {
                                logger.error("Can't parse RLMI notification", e);
                            }
                        }
                    }
                    
                    
                    /**
                     * PIDF PART
                     * 
                     */
                    ArrayList<String> pidfPartArray = multi.getPart("application/pidf-diff+xml");
                    try {
                        
                        if(pidfPartArray != null)
                        {
                        for (String pidfPart : pidfPartArray) 
                        {
                            // Parse PIDF part
                            InputSource pidfInput = new InputSource(new ByteArrayInputStream(pidfPart.getBytes()));
                            PidfParser pidfParser = new PidfParser(pidfInput);
                            PidfDocument presenceInfo = pidfParser.getPresence();
                            
                            handlePresenceInfo(presenceInfo);
                        }
                        }
                    } catch(Exception e) {
                        if (logger.isActivated()) {
                            logger.error("Can't parse PIDF notification", e);
                        }
                    }
                    
                }
                
                
            }
            catch(Exception e){
                if (logger.isActivated()) {
                    logger.error("Can't parse presence notification", e);
                }
            }
            
            
            //SUBSCRIPTION STATE HEADER
            // Check subscription state
            SubscriptionStateHeader stateHeader = (SubscriptionStateHeader)notify.getHeader(SubscriptionStateHeader.NAME);
            if ((stateHeader != null) && stateHeader.getState().equalsIgnoreCase("terminated")) {
                if (logger.isActivated()) {
                    logger.info("Presence subscription has been terminated by server");
                }
                //terminatedByServer();
            }
        }  
}



private void handlePresenceInfo(PidfDocument presence)
{
   if(presence!=null)
   {

        // Extract capabilities
       Capabilities capabilities =  new Capabilities();
       int registrationState =  ContactInfo.REGISTRATION_STATUS_UNKNOWN;
       // We queried via anonymous fetch procedure, so set presence discovery to true
       //capabilities.setPresenceDiscoverySupport(true);
       
        String contact = presence.getEntity();
        if (logger.isActivated()) {
            logger.debug("receiveCapNotification :" + contact);
        } 
        Vector<Tuple> tuples = presence.getTuplesList();
        
        if(tuples.size()>0)
        {
          //set the registration as true as the device published some capablity
            // We queried via anonymous fetch procedure, so set presence discovery to true
            registrationState =  ContactInfo.REGISTRATION_STATUS_ONLINE;
        }
        
        for(int i=0; i < tuples.size(); i++) {
            Tuple tuple = (Tuple)tuples.elementAt(i);
            boolean state = false; 
            if (tuple.getStatus().getBasic().getValue().equals("open")) {
                state = true;
            }
            String id = tuple.getService().getId();
            
            if (id.equalsIgnoreCase(PresenceUtils.FEATURE_RCS2_VIDEO_SHARE)) {
                capabilities.setVideoSharingSupport(state);
            } else
            if (id.equalsIgnoreCase(PresenceUtils.FEATURE_RCS2_VIDEO_SHARE)) {
                capabilities.setVideoSharingSupport(state);
            } else
            if (id.equalsIgnoreCase(PresenceUtils.FEATURE_RCS2_IMAGE_SHARE)) {
                capabilities.setImageSharingSupport(state);
            } else
            if (id.equalsIgnoreCase(PresenceUtils.FEATURE_RCS2_FT)) {
                //file transfer
                capabilities.setFileTransferSupport(state);
            } else
            if (id.equalsIgnoreCase(PresenceUtils.FEATURE_RCS2_CS_VIDEO)) {
                capabilities.setCsVideoSupport(state);
            } 
            else
            if (id.equalsIgnoreCase(PresenceUtils.FEATURE_RCS2_IP_VOICE_CALL)) {
            
                if(state)
                {
                    boolean audio = false;
                    boolean video = false;
                    boolean duplexmode = false;
                    
                  for(CapabilityDetails c : tuple.getServiceCapability().getCapabilityList())
                  {
                      //check for audio 
                      if(c.getName().equalsIgnoreCase("audio")){
                          if(c.getValue().equalsIgnoreCase("true"))
                          {
                              audio = true;
                              continue;
                          }
                      }
                      
                    //check for audio 
                      if(c.getName().equalsIgnoreCase("video")){
                          if(c.getValue().equalsIgnoreCase("true"))
                          {
                              video = true;
                              continue;
                          }
                      }
                      
                    //check for duplex
                      if(c.getName().equalsIgnoreCase("duplex")){
                          if(c.getValue().equalsIgnoreCase("full"))
                          {
                              duplexmode = true;
                              continue;
                          }
                      }

                  }
                  
                  if (logger.isActivated()) {
                           logger.debug("Video capability for : "+contact + "  - audio : "+audio + " ; video : " + video + " ; duplex :- " +duplexmode);
                  }
                  
                  capabilities.setIR94_VoiceCall(audio);
                  capabilities.setIR94_VideoCall(video);
                  capabilities.setIR94_DuplexMode(duplexmode);
                }

            }
            else
            if (id.equalsIgnoreCase(PresenceUtils.FEATURE_RCS2_DISCOVERY_VIA_PRESENCE)) {
                    capabilities.setPresenceDiscoverySupport(state);
            }
            else
            if ((id.equalsIgnoreCase(PresenceUtils.FEATURE_RCS2_CHAT))||
                (id.equalsIgnoreCase(PresenceUtils.FEATURE_RCS2_CHAT_2))
                ) {
                //IM capability
                capabilities.setImSessionSupport(state);
            }
        }

        if (logger.isActivated()) {
           logger.debug("Set Contact Capability via Presence : "+contact);
          }
        
        // Update capabilities in database
        ContactsManager.getInstance().setContactCapabilities(contact, capabilities, ContactInfo.RCS_CAPABLE, registrationState);

        // Notify listener
        imsModule.getCore().getListener().handleCapabilitiesNotification(contact, capabilities);
    
   }

  }

}
