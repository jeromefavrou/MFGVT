/*
                    GNU GENERAL PUBLIC LICENSE
                       Version 3, 29 June 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.
*/

#ifndef __GTKMMCOMPONENT_HPP__
#define __GTKMMCOMPONENT_HPP__

#include "../includes/gtkmmutilitys.hpp"
#include "../includes/device.hpp"

#include <gtkmm-3.0/gtkmm.h>
#include <gtkmm-3.0/gtkmm/viewport.h>

#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-page-renderer.h>

class PdfShower ;

/// @brief objet patron de partage par poiteur de donné essentiel
class TemplateGui : public utilitys::MainPathSharedTemplate
{
   public:
      TemplateGui(void);

      virtual void addCssProvider(Glib::RefPtr<Gtk::CssProvider> _cssProvider);
      virtual void addParent(Gtk::Window* _parent);
      virtual void addDevice(ptrVecDevice _devices);

      Glib::RefPtr<Gtk::CssProvider> const getCssProvider(void);
      Gtk::Window* const getParent(void);
      ptrVecDevice const getDevice(void);

      Glib::RefPtr<Gtk::CssProvider> const atCssProvider(void);
      Gtk::Window* const atParent(void);
      ptrVecDevice const atDevice(void);

      void addPdfShower( const std::shared_ptr< PdfShower > & _pdfS);
      const std::shared_ptr< PdfShower > &  atPdfShower( void) const;
      std::shared_ptr< PdfShower > &  getPdfShower( void) ;

      virtual ~TemplateGui(void);
      
      private :
protected :
      Gtk::Window* m_parent;

      Glib::RefPtr<Gtk::CssProvider> m_cssProvider;

      ptrVecDevice m_devices;
      std::shared_ptr< PdfShower > m_pdfS;

};

TemplateGui::TemplateGui(void):utilitys::MainPathSharedTemplate()
{
    this->m_parent = nullptr;
    this->m_pdfS = nullptr;
    this->m_devices = nullptr;
}

TemplateGui::~TemplateGui(void)
{
    this->m_parent = nullptr;
    this->m_pdfS = nullptr;
    this->m_devices = nullptr;
}

//setter
void TemplateGui::addCssProvider(Glib::RefPtr<Gtk::CssProvider> _cssProvider)
{
    this->m_cssProvider = _cssProvider;
}

//setter
void TemplateGui::addDevice( ptrVecDevice _devices )
{
    this->m_devices = _devices;
}

//setter
void TemplateGui::addParent(Gtk::Window* _parent)
{
    this->m_parent  = _parent;
}


//getter
Glib::RefPtr<Gtk::CssProvider> const TemplateGui::getCssProvider(void)
{
    return this->m_cssProvider;
}

//getter
Gtk::Window* const TemplateGui::getParent(void)
{
    return this->m_parent;
}

ptrVecDevice const TemplateGui::getDevice(void)
{
    return this->m_devices;
}

//acces securisé 
ptrVecDevice const TemplateGui::atDevice(void)
{
    if(!this->m_devices)
        throw LogicExceptionDialog("bad devices acces");

    return this->m_devices;
}
//acces securisé 
Glib::RefPtr<Gtk::CssProvider> const TemplateGui::atCssProvider(void)
{
    if(!this->m_cssProvider)
        throw LogicExceptionDialog("bad provider acces");

    return this->m_cssProvider;
}

//acces securisé 
Gtk::Window* const TemplateGui::atParent(void)
{
    if(this->m_parent == nullptr)
       throw LogicExceptionDialog("bad raw window acces");

    return this->m_parent;
}

void TemplateGui::addPdfShower( const std::shared_ptr< PdfShower > & _pdfS)
{
    this->m_pdfS = _pdfS;
}

const std::shared_ptr< PdfShower > &  TemplateGui::atPdfShower( void) const
{
    if(!this->m_pdfS)
        throw LogicExceptionDialog("bad PdfShower acces");

    return this->m_pdfS;
}

std::shared_ptr< PdfShower > &  TemplateGui::getPdfShower( void)
{
    return this->m_pdfS;
}


/// @brief objet de rendu d'un fichier pdf
class PdfShower : public TemplateGui , public Gtk::ScrolledWindow
{
    public:
    PdfShower(void );
    void read_pdf( const std::string & _file );
    void clear(void);


    private:
    std::vector<Gtk::Image> m_vec_images;
    std::vector<poppler::image> m_vec_pdfImage;
    Gtk::VBox m_bx;

    std::string m_current_file;

};

/// @brief initialisation de la fenetre de rendu
PdfShower::PdfShower(void ):TemplateGui(),Gtk::ScrolledWindow()
{
    this->set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    this->add(this->m_bx);

    this->m_current_file ="";
}

/// @brief lecture et affichage d'un pdf
/// @param _file chemin du ficheir pdf
void PdfShower::read_pdf( const std::string & _file )
{

    if( this->m_current_file == _file)
        return ;

    //lecture du pdf
    poppler::document *pdf_document = poppler::document::load_from_file( _file );
    
    // nettoyage de l'objet en cours
    this->clear();


    //verification de la validité du poiteur
    if( !pdf_document )
        throw LogicExceptionDialog( _file + " cannot be opened");


    //lecture des différente page 
    for(auto i = 0 ; i < (pdf_document->pages() <=10 ? pdf_document->pages() : 10 ); i++)
    {
        poppler::page * pdf_page = pdf_document->create_page(i);

        if(!pdf_page)
           throw LogicExceptionDialog( _file + " cannot be read page");
        

        this->m_vec_pdfImage.push_back(poppler::page_renderer().render_page(pdf_page));

        //transformation des donné brut en donné explatable gtkmm
        Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_data( (guint8*)this->m_vec_pdfImage.back().data(), Gdk::COLORSPACE_RGB, true, 8, this->m_vec_pdfImage.back().width(), this->m_vec_pdfImage.back().height(), this->m_vec_pdfImage.back().bytes_per_row() );

        //mis en memoir de la page pdf sous formt d'image
        this->m_vec_images.emplace_back(pixbuf);
        this->m_vec_images.back().show();
        
        this->m_bx.pack_start( this->m_vec_images.back() );

    }


    this->m_current_file = _file;
}

/// @brief netoyyage de l'objet en cour
void PdfShower::clear(void)
{
    utilitys::gtkmmRemoveChilds( this->m_bx );
    this->m_vec_images.clear();
    this->m_vec_pdfImage.clear();
}



/// @brief objet de rendu des groope de verion
class VersionShower :  public TemplateGui , public Gtk::VBox , public std::vector<GrpVersion>
{
    public:

        VersionShower(void);
        void update_view( void);

        void addCssProvider(Glib::RefPtr<Gtk::CssProvider> _cssProvider);

    private :

       void on_clic(GrpVersion _cpy);
       void update_complet_path(void);
       void clipBoardCpy(void);

       Gtk::VBox id  , nb_occur , m_grpInfoPath , m_grpInfoError , m_grpInfoPathLink , m_grpInfoDateCreate , m_grpInfoDateModif , m_grpInfoAutor, m_grpInfoVersion , m_grpInfoPart;
       GrpVersion m_current_sel;
       Gtk::Button  m_cpyPath;
       Gtk::Label m_createDate , m_modifDate , m_autor;

       Gtk::ComboBoxText m_device_sel , m_version_sel, m_ext_sel ,m_part_sel;
       Gtk::Entry m_complet_path;

       DialogThread m_dialogInfoGrp;

       bool m_onclickLock , m_auth_updatePath;
};


/// @brief initilisation de la fenetre de rendu
/// @param  
VersionShower::VersionShower(void):TemplateGui(),Gtk::VBox()
{
    this->m_onclickLock = false;
    this->m_auth_updatePath = false;
    const int colSize = 2;
    Gtk::Frame *tmp[colSize] ;

    //patie supperieur
    Gtk::Table *tab2 = Gtk::manage(new Gtk::Table( 2  , colSize ));  
    
    tab2->attach(*Gtk::manage(new Gtk::Label("id")), 0, 1, 0, 1 ,Gtk::SHRINK , Gtk::SHRINK); 
    tab2->attach(*Gtk::manage(new Gtk::Label("nb")), 1,2, 0, 1 , Gtk::SHRINK, Gtk::SHRINK);

    for(auto i =0 ; i < colSize ; i++)
    {
        tmp[i] =  Gtk::manage(new Gtk::Frame());

        if(i == 0)
            tab2->attach(*tmp[i], i, i+1, 1, 2 );
        else
            tab2->attach(*tmp[i], i, i+1, 1, 2 ,Gtk::SHRINK);
    }

    tmp[0]->add( this->id );
    tmp[1]->add( this->nb_occur);
    
        
    Gtk::ScrolledWindow *sw =Gtk::manage(new Gtk::ScrolledWindow());
    sw->add(*tab2);

    this->pack_start(*sw);

    //barr de selection inferieur
    Gtk::Table *tab3 = Gtk::manage(new Gtk::Table( 2  , 6 ));
    tab3->attach( *Gtk::manage(new Gtk::Label("CompletePath")), 0, 1, 0, 1 );
    tab3->attach( *Gtk::manage(new Gtk::Label("D")), 1, 2, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);
    tab3->attach( *Gtk::manage(new Gtk::Label("P")), 2, 3, 0, 1 , Gtk::SHRINK , Gtk::SHRINK); 
    tab3->attach( *Gtk::manage(new Gtk::Label("V")) , 3, 4, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);
    tab3->attach( *Gtk::manage(new Gtk::Label("E")), 4, 5, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);
    tab3->attach( *Gtk::manage(new Gtk::Label("")), 5, 6, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);

    tab3->attach( this->m_complet_path, 0, 1, 1, 2 );
    tab3->attach( this->m_device_sel, 1, 2, 1, 2 , Gtk::SHRINK , Gtk::SHRINK); 
    tab3->attach( this->m_part_sel, 2, 3, 1, 2 , Gtk::SHRINK , Gtk::SHRINK); 
    tab3->attach( this->m_version_sel , 3, 4, 1, 2 , Gtk::SHRINK , Gtk::SHRINK);
    tab3->attach( this->m_ext_sel, 4, 5, 1, 2 , Gtk::SHRINK , Gtk::SHRINK);
    tab3->attach( this->m_cpyPath, 5, 6, 1, 2 , Gtk::SHRINK , Gtk::SHRINK);

    this->m_complet_path.set_editable(false);

    this->m_version_sel.signal_changed().connect(  sigc::mem_fun(*this,&VersionShower::update_complet_path)  );
    this->m_ext_sel.signal_changed().connect(  sigc::mem_fun(*this,&VersionShower::update_complet_path)  );
    this->m_part_sel.signal_changed().connect(  sigc::mem_fun(*this,&VersionShower::update_complet_path)  );
    this->m_cpyPath.signal_clicked().connect(  sigc::mem_fun(*this,&VersionShower::clipBoardCpy)  );
    this->m_device_sel.signal_changed().connect(  sigc::mem_fun(*this,&VersionShower::update_complet_path)  );

    

    Gtk::Table *infoBare = Gtk::manage(new Gtk::Table( 2  , 3 ));

    infoBare->attach( *Gtk::manage(new Gtk::Label("Date of creation")) , 0, 1, 0, 1 );
    infoBare->attach(  *Gtk::manage(new Gtk::Label("Date of change")), 1, 2, 0, 1 ); 
    infoBare->attach(  *Gtk::manage(new Gtk::Label("Author")), 2, 3, 0, 1 ); 

    infoBare->attach(this->m_createDate , 0, 1, 1, 2 );
    infoBare->attach( this->m_modifDate, 1, 2, 1, 2 ); 
    infoBare->attach( this->m_autor, 2, 3, 1, 2 ); 

    this->pack_end(*infoBare , Gtk::PACK_SHRINK);
    
    this->pack_end(*tab3 , Gtk::PACK_SHRINK);


    this->m_dialogInfoGrp.signal_hide().connect(sigc::mem_fun( this->m_dialogInfoGrp, &DialogThread::stop));
    this->m_dialogInfoGrp.signal_show().connect(sigc::mem_fun( this->m_dialogInfoGrp, &DialogThread::run));
    this->m_dialogInfoGrp.set_title( "Grp Info");
    this->m_dialogInfoGrp.set_resizable(true  );

    this->m_dialogInfoGrp.set_default_size(700, 480);
    this->m_dialogInfoGrp.maximize();
    
    this->m_dialogInfoGrp.set_position(Gtk::WIN_POS_CENTER);
    Gtk::Table *infoGrp = Gtk::manage(new Gtk::Table( 2  , 8 ));

    infoGrp->attach( *Gtk::manage(new Gtk::Label("Error")) , 0, 1, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);
    infoGrp->attach( *Gtk::manage(new Gtk::Label("Path File")) , 1, 2, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);
    infoGrp->attach( *Gtk::manage(new Gtk::Label("Path Link")) , 2,3, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);
    infoGrp->attach( *Gtk::manage(new Gtk::Label("Date of creation")) , 3, 4, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);
    infoGrp->attach( *Gtk::manage(new Gtk::Label("Date of change")) , 4, 5, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);
    infoGrp->attach( *Gtk::manage(new Gtk::Label("Author")) , 5, 6, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);
    infoGrp->attach( *Gtk::manage(new Gtk::Label("Version")) , 6, 7, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);
    infoGrp->attach( *Gtk::manage(new Gtk::Label("Part")) , 7, 8, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);

    infoGrp->attach( this->m_grpInfoError , 0, 1, 1, 2 ,Gtk::SHRINK , Gtk::SHRINK);
    infoGrp->attach( this->m_grpInfoPath ,1, 2, 1, 2 ,Gtk::SHRINK , Gtk::SHRINK);
    infoGrp->attach( this->m_grpInfoPathLink ,2, 3, 1, 2 ,Gtk::SHRINK , Gtk::SHRINK);
    infoGrp->attach( this->m_grpInfoDateCreate ,3, 4, 1, 2 ,Gtk::SHRINK , Gtk::SHRINK);
    infoGrp->attach( this->m_grpInfoDateModif ,4, 5, 1, 2 ,Gtk::SHRINK , Gtk::SHRINK);
    infoGrp->attach( this->m_grpInfoAutor ,5, 6, 1, 2 ,Gtk::SHRINK , Gtk::SHRINK);
    infoGrp->attach( this->m_grpInfoVersion ,6, 7, 1, 2 ,Gtk::SHRINK , Gtk::SHRINK);
    infoGrp->attach( this->m_grpInfoPart ,7, 8, 1, 2 ,Gtk::SHRINK , Gtk::SHRINK);


    Gtk::ScrolledWindow *sw2 =Gtk::manage(new Gtk::ScrolledWindow());
    sw2->set_policy(Gtk::PolicyType::POLICY_AUTOMATIC, Gtk::PolicyType::POLICY_AUTOMATIC);
    sw2->add(*infoGrp);
    sw2->set_vexpand(true);
    this->m_dialogInfoGrp.get_content_area()->add( *sw2 );
}

/// @brief réecriture du poiteur provider
/// @param _cssProvider ptr provider
void VersionShower::addCssProvider(Glib::RefPtr<Gtk::CssProvider> _cssProvider)
{
    this->m_cssProvider = _cssProvider;

    this->get_style_context()->add_class( "VersionShower" );
    this->get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);

    this->m_complet_path.get_style_context()->add_class( "VersionShowerEntry" );
    this->m_complet_path.get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);

    this->m_cpyPath.get_style_context()->add_class( "VersionShowerButtonPP" );
    this->m_cpyPath.get_style_context()->add_class( "Button" );
    this->m_cpyPath.get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);


    this->m_device_sel.get_style_context()->add_class( "VersionShowerButtonDevSel" );
    this->m_device_sel.get_style_context()->add_class( "ComboTextButton" );
    this->m_device_sel.get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);

    this->m_version_sel.get_style_context()->add_class( "VersionShowerButtonVerSel" );
    this->m_version_sel.get_style_context()->add_class( "ComboTextButton" );
    this->m_version_sel.get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    this->m_ext_sel.get_style_context()->add_class( "VersionShowerButtonExtSel" );
    this->m_ext_sel.get_style_context()->add_class( "ComboTextButton" );
    this->m_ext_sel.get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);

    this->m_part_sel.get_style_context()->add_class( "VersionShowerButtonPartSel" );
    this->m_part_sel.get_style_context()->add_class( "ComboTextButton" );
    this->m_part_sel.get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER); 

}

/// @brief ajoute au presse papier la selection en cours
/// @param  
void VersionShower::clipBoardCpy(void)
{
    if(this->m_current_sel.size() == 0)
        return ;

    utilitys::cpyToPP(  this->m_complet_path.get_text() );

}

/// @brief met a jour les groupe a affiché
/// @param  
void VersionShower::update_view( void)
{
    //nettoys la zone pdf
    this->atPdfShower()->clear();
    
    //nettoye tout les enfant gtkmm 
    utilitys::gtkmmRemoveChilds(this->id);
    utilitys::gtkmmRemoveChilds(this->nb_occur);

    //recré les enfant gtkmm au rapport au groupe actuels
    for(GrpVersion & grp : *this)
    {
        try
        {
            auto bp = Gtk::manage(new Gtk::Button(  grp.get_id() , Gtk::PACK_SHRINK ));

            bp->signal_clicked().connect(sigc::bind<GrpVersion>(sigc::mem_fun(*this,&VersionShower::on_clic), grp));

            bp->get_style_context()->add_class( "VersionShowerButtonId" );
            bp->get_style_context()->add_class( "Button" );

            if( grp.inError() )
            {
                bp->get_style_context()->add_class( "VersionShowerButtonIdKo" );
            }
            else
            {
                bp->get_style_context()->add_class( "VersionShowerButtonIdOk" );
            }
            
            bp->get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);

            this->id.pack_start(*bp,Gtk::PACK_SHRINK);


            auto lab2 = Gtk::manage(new Gtk::Button( utilitys::ss_cast<size_t , std::string>( grp.size() ) , Gtk::PACK_SHRINK ));

            lab2->get_style_context()->add_class( "VersionShowerButtonOccur" );
            lab2->get_style_context()->add_class( "Button" );
            lab2->get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);

            lab2->signal_clicked().connect(sigc::bind<GrpVersion>(sigc::mem_fun(*this,&VersionShower::on_clic), grp));
            lab2->signal_clicked().connect(  sigc::mem_fun(this->m_dialogInfoGrp ,&DialogThread::show_all)  );
            this->nb_occur.pack_start(*lab2,Gtk::PACK_SHRINK);
        }
        catch(const std::exception& e)
        {
            const LogicExceptionDialog er(e.what());
            er.show();

            continue;
        }
        
       
    }


    this->show_all();
}


/// @brief met a jour la selection suite a un clique sur un des bouton de selection
/// @param _cpy 
void VersionShower::on_clic(GrpVersion _cpy)
{
    //deja un signal en cour
    if(this->m_onclickLock)
        return ;

    const auto start{std::chrono::steady_clock::now()};

    this->m_auth_updatePath = false;

    this->m_onclickLock = true;
    this->m_current_sel = _cpy;


    //on ajoute les device existante 
    this->m_device_sel.remove_all();

    std::vector<std::string> tmp;

    for( const auto & vers : _cpy)
    {
        if(vers.get_error() != 0)
            continue;
        
        if(  vers.device.size() > 0 ) 
        {   
            bool found = false;

            for(const auto & str : tmp)
            {
                if(str == vers.device  )
                {
                    found = true ;
                    break;
                }
            }

            if(!found )
                tmp.emplace_back( vers.device );
        }
    }

     for(const auto & str : tmp)
     {
        this->m_device_sel.append(str);
     }

    
    if(tmp.size() > 0)
    {
         this->m_device_sel.set_active_text(tmp.front());
         tmp.clear();
    }

    
    //on ajoute les extension existante 
    this->m_ext_sel.remove_all();


    for( const auto & vers : _cpy)
    {
        auto tmpvers = vers.is_lnk() ? vers.ptr_lnk.get() : &vers;

        if(  tmpvers->get_ext().size() > 0 ) 
        {   
            bool found = false;

            for(const auto & str : tmp)
            {
                if(str == tmpvers->get_ext()  )
                {
                    found = true ;
                    break;
                }
            }

            if(!found)
                tmp.emplace_back( tmpvers->get_ext() );
        }
    }

     for(const auto & str : tmp)
     {
        this->m_ext_sel.append(str);
     }

    
    if(tmp.size() > 0)
    {
         this->m_ext_sel.set_active_text(tmp.front());
         tmp.clear();
    }
   

    
    //ajoute les version existante
    this->m_version_sel.remove_all();
    for( const auto & vers : _cpy)
    {
        auto tmpvers = vers.is_lnk() ? vers.ptr_lnk.get() : &vers;

        if(  tmpvers->version.size() > 0 ) 
        {
            bool found = false;
            for(const auto & str : tmp)
            {
                if(str == tmpvers->version)
                {
                    found = true ;
                    break;
                }
            }

            if(!found  )
                tmp.emplace_back( tmpvers->version );
        }
    }

     for(const auto & str : tmp)
        this->m_version_sel.append(str);
     
     
    if(tmp.size() > 0)
    {
        std::sort(tmp.rbegin(), tmp.rend());

        this->m_version_sel.set_active_text( tmp.front() );
        tmp.clear();
    }
    
    
    //ajoute les partie existante
    this->m_part_sel.remove_all();
    for( const auto & vers : _cpy)
    {
        auto tmpvers = vers.is_lnk() ? vers.ptr_lnk.get() : &vers;

        if(  tmpvers->part.size() > 0 ) 
        {
            bool found = false;
            for(const auto & str : tmp)
            {
                if(str == tmpvers->part)
                {
                    found = true ;
                    break;
                }
            }

            if(!found )
                tmp.emplace_back( tmpvers->part );
        }
    }

     for(const auto & str : tmp)
        this->m_part_sel.append(str);
     
     

    if(tmp.size() > 0)
    {
        std::sort(tmp.rbegin(), tmp.rend());

        this->m_part_sel.set_active_text( tmp.back() );
        tmp.clear();
    }
    

    //completel'infobare

    this->m_auth_updatePath = true;

    this->update_complet_path();


    utilitys::gtkmmRemoveChilds(this->m_grpInfoPath);
    utilitys::gtkmmRemoveChilds(this->m_grpInfoError);
    utilitys::gtkmmRemoveChilds(this->m_grpInfoPathLink);
    utilitys::gtkmmRemoveChilds(this->m_grpInfoDateCreate);
    utilitys::gtkmmRemoveChilds(this->m_grpInfoDateModif);
    utilitys::gtkmmRemoveChilds(this->m_grpInfoAutor);
    utilitys::gtkmmRemoveChilds(this->m_grpInfoPart);
    utilitys::gtkmmRemoveChilds(this->m_grpInfoVersion);

    for(  auto & vers : this->m_current_sel)
    {
       
        auto tmp1 =Gtk::manage( new Gtk::Entry( ));
        tmp1->set_text( vers.get_file());
        tmp1->set_editable(false);
        tmp1->get_style_context()->add_class( "VersionShowerGrpInfoPath" );

        if( vers.get_error() & Version::EF_ACCES ||  vers.get_error() & Version::EF_LNK || vers.get_error() & Version::EF_MD5)
            tmp1->get_style_context()->add_class( "VersionShowerGrpKo" );

        tmp1->get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);

        this->m_grpInfoPath.pack_start(*tmp1 ,Gtk::PACK_SHRINK);

        auto tmp2 =Gtk::manage( new Gtk::Button( ));

        tmp2->set_label( Version::VersionError(vers) + (vers.is_lnk() ? Version::VersionError(*vers.ptr_lnk) : "") ) ;

        tmp2->get_style_context()->add_class( "VersionShowerGrpInfoError" );
        tmp2->get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);

        this->m_grpInfoError.pack_start(*tmp2,Gtk::PACK_SHRINK);


        auto tmp3 =Gtk::manage( new Gtk::Entry( ));
        tmp3->set_text(vers.is_lnk() ? vers.ptr_lnk->get_file() : "");
        tmp3->set_editable(false);

        tmp3->get_style_context()->add_class( "VersionShowerGrpInfoPathLink" );

        if(vers.is_lnk())
            if( vers.ptr_lnk->get_error() & Version::EF_ACCES ||  vers.ptr_lnk->get_error() & Version::EF_LNK || vers.ptr_lnk->get_error() & Version::EF_MD5)
                tmp3->get_style_context()->add_class( "VersionShowerGrpKo" );

        tmp3->get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);

        this->m_grpInfoPathLink.pack_start(*tmp3,Gtk::PACK_SHRINK);


        auto tmp4 =Gtk::manage( new Gtk::Button( vers.is_lnk() ? vers.ptr_lnk->createDate : vers.createDate ));

        tmp4->get_style_context()->add_class( "VersionShowerGrpInfoCreateDate" );
        if( vers.get_error() & Version::EF_CreateDate  )
            tmp4->get_style_context()->add_class( "VersionShowerGrpKo" );
        else if(vers.is_lnk())
            if( vers.ptr_lnk->get_error() & Version::EF_CreateDate  )
                tmp4->get_style_context()->add_class( "VersionShowerGrpKo" );
        
        tmp4->get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);

        this->m_grpInfoDateCreate.pack_start(*tmp4,Gtk::PACK_SHRINK);


        auto tmp5 =Gtk::manage( new Gtk::Button( vers.is_lnk() ? vers.ptr_lnk->modifDate : vers.modifDate ));

        tmp5->get_style_context()->add_class( "VersionShowerGrpInfoModifDate" );
        if( vers.get_error() & Version::EF_ModifDate)
            tmp5->get_style_context()->add_class( "VersionShowerGrpKo" );
        else if(vers.is_lnk())
            if( vers.ptr_lnk->get_error() & Version::EF_ModifDate  )
                tmp5->get_style_context()->add_class( "VersionShowerGrpKo" );

        tmp5->get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);

        this->m_grpInfoDateModif.pack_start(*tmp5,Gtk::PACK_SHRINK);


        auto tmp6 =Gtk::manage( new Gtk::Button( vers.is_lnk() ? vers.ptr_lnk->autor : vers.autor ));

        tmp6->get_style_context()->add_class( "VersionShowerGrpInfoAuthor" );
        tmp6->get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);

        this->m_grpInfoAutor.pack_start(*tmp6,Gtk::PACK_SHRINK);


        auto tmp7 =Gtk::manage( new Gtk::Button( vers.is_lnk() ? vers.ptr_lnk->part : vers.part ));

        tmp7->get_style_context()->add_class( "VersionShowerGrpInfoPart" );
        tmp7->get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);

        this->m_grpInfoPart.pack_start(*tmp7,Gtk::PACK_SHRINK);


        auto tmp8 =Gtk::manage( new Gtk::Button( vers.is_lnk() ? vers.ptr_lnk->version : vers.version ));

        tmp8->get_style_context()->add_class( "VersionShowerGrpInfoVersion" );
        tmp8->get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);

        this->m_grpInfoVersion.pack_start(*tmp8,Gtk::PACK_SHRINK);

    }


    this->m_onclickLock = false;


    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};

    std::clog <<"succes selected in : " << elapsed_seconds.count() * 1000 << " ms" <<std::endl;

}

/// @brief met a jour la chemin complet vers le fichier selectionné , et si un pdf , l'affiche
/// @param  
void VersionShower::update_complet_path(void)
{
    if( !this->m_auth_updatePath )
        return ;

    
    for(auto & vers : this->m_current_sel)
    {
        auto tmpvers = vers.is_lnk() ? vers.ptr_lnk.get() : &vers;
        if( this->m_device_sel.get_active_text() == tmpvers->device && "pdf" == tmpvers->get_ext()  && this->m_part_sel.get_active_text() == tmpvers->part && this->m_version_sel.get_active_text() == tmpvers->version)
        {   
            try
            {
                //lit pdf et l'affiche 
                
                this->atPdfShower()->read_pdf( tmpvers->get_file());

                this->atPdfShower()->show_all();
            }
            catch( LogicExceptionDialog const & e)
            {
                e.show();
            }
            catch(const std::exception& e)
            {
                vers.add_error(Version::EF_ACCES);
                std::cerr << e.what() << '\n';
            }
        }

        ///remlpis le chemin compelt
        if( this->m_device_sel.get_active_text() == tmpvers->device && this->m_ext_sel.get_active_text() == tmpvers->get_ext()  && this->m_part_sel.get_active_text() == tmpvers->part && this->m_version_sel.get_active_text() == tmpvers->version)
        {
            
            this->m_complet_path.set_text( tmpvers->get_file());

            this->m_autor.set_label(tmpvers->autor );
            this->m_createDate.set_label( tmpvers->createDate );
            this->m_modifDate.set_label( tmpvers->modifDate );
            
            return ;
        }
    }

    //remet a vide le champs si rien de trouver
    this->m_complet_path.set_text("");

    this->m_autor.set_label( "" );
    this->m_createDate.set_label( "");
    this->m_modifDate.set_label( "" );
}




/// @brief class de gestion des bouton d'autocompletion
class AutoCompletVisualiz :  public TemplateGui , public Gtk::ScrolledWindow
{
    public:
         AutoCompletVisualiz(void);

         void update(std::vector<std::string> const & list);

         sigc::signal<void ,std::string> & signal_select(void);

         void addCssProvider(Glib::RefPtr<Gtk::CssProvider> _cssProvider);

    private:
    Gtk::HBox m_hbox;
    std::vector<Gtk::Button> m_button;

    void signal_clicked(std::string const & value);


    sigc::signal<void , std::string > m_signalSelect;

};

/// @brief initilalize
/// @param  
AutoCompletVisualiz::AutoCompletVisualiz(void):TemplateGui(), Gtk::ScrolledWindow()
{
    this->add( this->m_hbox );
}

void AutoCompletVisualiz::addCssProvider(Glib::RefPtr<Gtk::CssProvider> _cssProvider)
{
    this->m_cssProvider = _cssProvider;
    this->get_style_context()->add_class( "AutoCompletVisualizeScroll" );

    this->get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);


    this->m_hbox.get_style_context()->add_class( "AutoCompletVisualizeBar" );
    this->m_hbox.get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);
}

/// @brief emmet un signal si clique
/// @param value str a envoyer
void AutoCompletVisualiz::signal_clicked(std::string const & value)
{
    this->m_signalSelect.emit(value);
}

 /// @brief signal si selectionné
 /// @return retourn le signal
 sigc::signal<void ,std::string> & AutoCompletVisualiz::signal_select(void)
 {
    return this->m_signalSelect;
 }

/// @brief met a jour la liste des bouton d'autocompletion
/// @param list 
void AutoCompletVisualiz::update(std::vector<std::string> const & list)
{
    for(auto it = this->m_button.begin() ; it != this->m_button.end() ; it++)
        this->m_hbox.remove(*it);
    

    this->m_button.clear();

    for(size_t i =0 ; i <  list.size() ;i++)
    {
        this->m_button.push_back( Gtk::Button(list[i]));
        

        this->m_button.back().set_can_focus(false);
        this->m_button.back().set_focus_on_click(false);
        this->m_button.back().signal_clicked().connect( sigc::bind<std::string>(sigc::mem_fun(*this, &AutoCompletVisualiz::signal_clicked),list[i]));
    
        this->m_button.back().get_style_context()->add_class( "AutoCompletVisualizeButton" );
        this->m_button.back().get_style_context()->add_class( "Button" );
        this->m_button.back().get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);
        this->m_hbox.pack_start( this->m_button.back() ,  Gtk::PACK_SHRINK);
    }

    this->m_hbox.show_all();
}


/// @brief class quui gerer le(s) prompt/commande de départ
class Prompt  :  public TemplateGui , public Gtk::VBox
{
    public : 

        Prompt(void);

        void addCssProvider(Glib::RefPtr<Gtk::CssProvider> _cssProvider);
 
    private:
        void completionList(void);
        void completion(std::string const & value);
        void enterSignal(void);

        void helpSignal(void);

        int findDualChar(std::string const & line , const char c1 , const char c2);

        void update_th( std::vector< std::string >::iterator const & _beg , std::vector< std::string >::iterator const & _end , std::vector<std::string > const & lsAtIgnor , std::vector< File > & files) ; 
        void showgrp_th( std::vector< std::string >::iterator const & _beg , std::vector< std::string >::iterator const & _end , std::vector<std::string > const & lsAtIgnor ) ; 
        void update_showgrp_th( std::vector< VContainer >::iterator const & _beg , std::vector< VContainer >::iterator const & _end);

        Gtk::Entry m_entry;
        AutoCompletVisualiz m_autoCompletion;
        SwitchUser m_swu_update;
        Gtk::Button m_help;
};


/// @brief initialise la bare de prompt
/// @param  
Prompt::Prompt(void):TemplateGui() ,  Gtk::VBox()
{
    auto hbox = Gtk::manage( new Gtk::HBox() );

    //champt d'entrée
    this->m_entry.set_can_focus(true);
    this->m_entry.set_focus_on_click(true);
    this->m_entry.grab_focus();
    this->m_entry.set_alignment(Gtk::ALIGN_START);
    this->m_entry.set_placeholder_text("Entrer un materiel");

    this->m_entry.set_max_length(200);
    this->m_entry.signal_activate().connect(sigc::mem_fun(*this, &Prompt::enterSignal));
    this->m_entry.signal_changed().connect( sigc::mem_fun( *this , &Prompt::completionList ));

    //barre de bp d'auto completion
    this->m_autoCompletion.signal_select().connect( sigc::mem_fun( *this , &Prompt::completion ));

    //bouton d'aide
    this->m_help.signal_clicked().connect( sigc::mem_fun( *this , &Prompt::helpSignal));


    //bouton de mise a jour continue
    this->m_swu_update.set_label("update");
    this->m_swu_update.set_active(false);
    this->m_swu_update.signal_changed().connect(sigc::mem_fun(*this, &Prompt::enterSignal));

    
    hbox->pack_start(this->m_entry);
    hbox->pack_start(this->m_swu_update , Gtk::PACK_SHRINK);
    hbox->pack_start(this->m_help, Gtk::PACK_SHRINK);

    this->pack_start(this->m_autoCompletion);
    this->pack_start(*hbox);
}

/// @brief reecriture de l'ajout du ptr css provider
/// @param _cssProvider pointeur
void Prompt::addCssProvider(Glib::RefPtr<Gtk::CssProvider> _cssProvider)
{
    this->m_cssProvider = _cssProvider;

    this->m_autoCompletion.addCssProvider(this->getCssProvider());

    this->get_style_context()->add_class( "PromptBar" );

    this->get_style_context()->add_provider( this->getCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);

    this->m_entry.get_style_context()->add_class( "PromptEntry" );
    this->m_entry.get_style_context()->add_provider( this->getCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);

    this->m_help.get_style_context()->add_class( "PromptHelp" );
    this->m_help.get_style_context()->add_class( "Button" );
    this->m_help.get_style_context()->add_provider( this->getCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);


    this->m_swu_update.get_style_context()->add_class( "SwitwhButton" );
    this->m_swu_update.get_style_context()->add_provider( this->getCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);

}

/// @brief fonction a executeru au signal d'aide , ouvre une page web
void Prompt::helpSignal(void)
{
    utilitys::openWebPage("https://github.com/jeromefavrou/MFVGT/wiki");
}

/// @brief met a jour les information des version des groupe des materiel , fonction threadable
/// @param _beg pointeur de depart
/// @param _end pointeur de fin
/// @param lsAtIgnor liste a ignoré
/// @param files liste de fichieir a traité
void Prompt::update_th( std::vector< std::string >::iterator const & _beg , std::vector< std::string >::iterator const & _end , std::vector<std::string > const & lsAtIgnor , std::vector< File > & files)
{
    for(auto lsDev = _beg ; lsDev!= _end ; lsDev++)
    {
        std::string current = *lsDev;

        bool recurs = current.back() == '*' ? true : false;

        if(recurs)
            current.erase(current.end()-1);

        for(auto & dev : *this->atDevice() )
        {
            bool maybeIgnor = false;

            for(auto const & lsI : lsAtIgnor )
            {
                bool recursBan = lsI.back() == '*' ? true : false;

                if( (dev.get_name() == lsI  && !recursBan) || (dev.get_name().find(lsI.substr(0 , lsI.size() -1 ) ) != std::string::npos && recursBan) )
                {
                    maybeIgnor = true;

                    break;
                }
            }

            if(maybeIgnor)
                continue;

            if( std::find(lsAtIgnor.begin(), lsAtIgnor.end(), dev.get_name()) != lsAtIgnor.end() )
                continue ;
            
            if( (dev.get_name() ==  current && !recurs ) || (dev.get_name().find(current) != std::string::npos && recurs))
            {
                dev.update( files , this->m_swu_update.get_active() , std::thread::hardware_concurrency()/2 );

                if(!recurs)
                    break;
            }
        }
    }
}


/// @brief ajoute les groupe a affiché , fonction threadable
/// @param _beg pointeur de depart
/// @param _end pointeur de fin
/// @param lsAtIgnor liste a ignoré
void Prompt::showgrp_th( std::vector< std::string >::iterator const & _beg , std::vector< std::string >::iterator const & _end , std::vector<std::string > const & lsAtIgnor )
{
    for(auto lsDev = _beg ; lsDev!= _end ; lsDev++)
    {
        std::string current = *lsDev;

        bool recurs = current.back() == '*' ? true : false;

        if(recurs)
            current.erase(current.end()-1);

        for(auto & dev : *this->atDevice() )
        {

            bool maybeIgnor = false;

            for(auto const & lsI : lsAtIgnor )
            {
                bool recursBan = lsI.back() == '*' ? true : false;

                if( (dev.get_name() == lsI  && !recursBan) || (dev.get_name().find(lsI.substr(0 , lsI.size() -1 ) ) != std::string::npos && recursBan) )
                {
                    maybeIgnor = true;

                    break;
                }
            }

            if(maybeIgnor)
                continue;

            if( (dev.get_name() ==  current  && !recurs ) || ( dev.get_name().find(current) != std::string::npos && recurs))
                for( auto & cont : dev.get_containers() )
                    for(auto & grp : cont)
                        cont.atVersionShower()->push_back(grp);
        }
    }
}

/// @brief concatene les groupes et verifi les incoérence, fonction threadable
/// @param _beg poiteur de depart
/// @param _end pointeur de fin
void Prompt::update_showgrp_th( std::vector< VContainer >::iterator const & _beg , std::vector< VContainer >::iterator const & _end)
{
    for( auto  cont = _beg ; cont != _end ; cont++)
    {
        // fusionne les groupeVersion
        for( auto it = cont->atVersionShower()->begin(); it != cont->atVersionShower()->end() ; it++ )
        {
            if(it +1 <  cont->atVersionShower()->end())
            {
                std::string id1 = "";

                try
                {
                   id1 = it->get_id();
                }
                catch(std::exception const & e)
                {
                   auto tmpIt = it;
                   it= it-1;

                   cont->atVersionShower()->erase(tmpIt);
                   
                   const LogicExceptionDialog er( std::string(e.what()) + " merging grp ignored and error grp erase");

                   continue;
                }


                for( auto it2 = it+1 ; it2 != cont->atVersionShower()->end() ; it2++ )
                {
                    try
                    {
                        if( id1 == it2->get_id() )
                        {
                            //fusionne les groupes
                            it->merge(*it2);
                            it=cont->atVersionShower()->begin()-1;
                            cont->atVersionShower()->erase(it2);
                            break;
                        }
                    }
                    catch(std::exception const & e)
                    {
                        std::cerr << e.what() << " merging grp ignored and error grp erase"<<std::endl;

                        cont->atVersionShower()->erase(it2);

                        continue;
                    }
                   
                } 
            }
        }

        //verifie si il a des incoherence entre fichier
        for(auto & grpShow : *cont->atVersionShower())
        {
            grpShow.addMainPath( this->getMainPath() );
            grpShow.check( std::thread::hardware_concurrency() /2 );
        }


        //mise a jour des vue
        cont->atVersionShower()->update_view();

    }
}

/// @brief fonction utiliser par le signal de validation du champd de prommpt
void Prompt::enterSignal(void)
{
    const auto start{std::chrono::steady_clock::now()};

    std::vector< std::string > lsAtSearch , lsAtIgnor;
    
    //recuperre le prompte
    std::string && current_entry = this->m_entry.get_text();

    if(current_entry.size() ==0)
        return;

    //crée la liste de recherche et la aliste a ignoré
    auto res = utilitys::sep_string<';'>( current_entry );
    lsAtSearch.push_back( std::move(std::get<0>(res)) );

    while( std::get<1>(res).size() > 0 )
    {
        res = utilitys::sep_string<';'>( std::get<1>(res) );

        if( std::get<0>(res).front() == '!' )
            lsAtIgnor.push_back( std::get<0>(res).substr(1 , std::get<0>(res).size() )) ;
        
        else
            lsAtSearch.push_back( std::move(std::get<0>(res)) );
        
    }


    std::vector< File > files ;
    // Utiliser la fonction récursive pour lister l'enseble des fichiers du repertoir

    try
    {
        utilitys::listerFichiers(*this->atMainPath(), files);
    }
    catch(std::exception const & _e)
    {
        files.clear();
        const LogicExceptionDialog er(_e.what());
    }


    //update
    utilitys::multi_thread_callback( std::bind( &Prompt::update_th , this , std::placeholders::_1 , std::placeholders::_2  , lsAtIgnor , std::ref(files)) , lsAtSearch , std::thread::hardware_concurrency()/2); 
    

    //nettoyage des vue
    for( auto & cont : this->atDevice()->front().get_containers() )
        cont.atVersionShower()->clear();
    
    //ajoute les groupe a afficher
    utilitys::multi_thread_callback( std::bind( &Prompt::showgrp_th , this , std::placeholders::_1 , std::placeholders::_2  , lsAtIgnor) , lsAtSearch , std::thread::hardware_concurrency()/2); 

    //mise a jour des vue
    utilitys::multi_thread_callback( std::bind( &Prompt::update_showgrp_th , this , std::placeholders::_1 , std::placeholders::_2  ) , this->atDevice()->front().get_containers() , 0); 
    
    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};

    std::clog <<"succes load entry in : " << elapsed_seconds.count() * 1000 << " ms" <<std::endl;
    
}

/// @brief fonction appler par le sigal d'autocompletion du champ
/// @param value valeur proposé d'autocompletion
void Prompt::completion(std::string const & value)
{
    std::string _prompt = this->m_entry.get_text();
    std::string last;

     //garde que le dernier segment ( separer par ; )
    for(auto i = _prompt.size() ;i >0 ; i--)
    {
        if( _prompt[i] == ';')
        {
            last = std::string(_prompt.begin() ,_prompt.begin() + i +1);
            _prompt.erase(_prompt.begin() ,_prompt.begin() + i +1 );
            break;
        }
    }


    if( _prompt.front() == '!' )
        _prompt = "!";
    else
        _prompt = "";


    this->m_autoCompletion.update({});
    this->m_entry.set_text(last + _prompt + value);
    this->m_entry.set_position(this->m_entry.get_text_length());
    this->m_entry.activate();
}

/// @brief recompose la liste des autocompletion et garantie la casse et les contrainte de caractere
void Prompt::completionList(void)
{
    std::string currentText = this->m_entry.get_text(); 

    if(currentText.size() == 0)
        return ;

     
    //permission de caractere
    for(std::string::iterator it = currentText.begin() ; it != currentText.end() ; it++ )
    {
        if(currentText.size() == 0)
            break;
        
        bool dec =  *it >= 0x30 && *it <= 0x39;
        bool maj =  *it >= 0x41 && *it <= 0x5A;
        bool min =  *it >= 0x61 && *it <= 0x7A;
        bool star = *it == '*' || *it == ';' || *it == '!';
        
        if( !dec && !maj && !min && !star)
        {
            currentText.erase(it);
            it = currentText.begin() ;
        }

        //upper
        if(min)
            *it = *it - char(32) ;
        
    }


    //mise a jour du champs de prompt
    this->m_entry.set_text(currentText);
    
    if( currentText.size() == 0 || currentText.back() =='*')
        return ;

    //recupere la dernier valaur du prompt
    currentText = std::get<1>(  utilitys::sep_string<';'>(currentText, true) );

    if(currentText.front() == '!')
    {
        currentText.erase( currentText.begin()); 
    }

    std::vector<std::string> autoComplet;

    //recré la liste d'autocompletion
    for(auto & device : *this->m_devices)
    {
        std::string tmp  =device.get_name();
        std::size_t found = tmp.find( currentText );
        // si trouver a l'index 0
        if (found!=std::string::npos && found == 0)
        {
            //verifie que pas deja present
             auto it = std::find( autoComplet.begin(),autoComplet.end() ,tmp );

             if(it == autoComplet.end())
                autoComplet.push_back(tmp);
        }
    }

    //mise a jour de la liste
    this->m_autoCompletion.update(autoComplet);

}


/// @brief gestion de la fenetre principa
class WindowMain : public Gtk::Window , public TemplateGui
{
    public:

        WindowMain( const std::string & _absPath );

        void init(void);
        const std::string get_ressourcePath(void)const;
        
    protected :

    private:

        Gtk::HPaned m_utilitysPanel;
        Gtk::VBox m_usersPanel;
        Gtk::Notebook m_nbk;
        Prompt m_prompt;

        std::string m_absPath , m_ressourcePath;

        Glib::RefPtr< Gtk::Adjustment > m_vadj , m_hadj ;

};

/// @brief 
/// @param _absPath 
WindowMain::WindowMain(const std::string & _absPath) : Gtk::Window() , TemplateGui() 
{
    this->m_absPath = _absPath;
    this->m_parent = this;
    //this->addMainPath( std::shared_ptr< const std::string >(new const std::string(_mainPath)) );

    auto && path = utilitys::readList(this->m_absPath + "\\ressourcePath.csv" , false);

    if( path.size() == 0 )
         throw std::logic_error("no ressourcePath found");

    this->m_ressourcePath = path.front();
}

const std::string WindowMain::get_ressourcePath(void)const
{
    return this->m_ressourcePath;
}

void WindowMain::init(void)
{
    this->m_prompt.addDevice(this->m_devices);
    this->m_prompt.addMainPath( this->atMainPath() );

    
    try
    {
       this->set_icon_from_file(  this->m_ressourcePath+ "\\version-control-lrg.png");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    catch(Gdk::PixbufError & e)
    {
        std::cerr << e.what() << '\n';
    }
    
    

    this->unfullscreen();
	this->set_position(Gtk::WIN_POS_CENTER);
	this->resize(1200, 480);
    this->maximize();

    this->addCssProvider( Gtk::CssProvider::create() );
    this->m_prompt.addCssProvider( this->atCssProvider() );

    //chargement du fichier css

    std::string const cssFile =  this->m_ressourcePath + "\\style.css" ;

    try
    {
        this->atCssProvider()->load_from_path( cssFile );
    }
    catch(Gtk::CssProviderError const & e)
    {
        const LogicExceptionDialog er(cssFile + " cannot be load : " + e.what());
        er.show();
    }

    //class pour fichier css
    this->get_style_context()->add_class("windows");
    this->m_utilitysPanel.get_style_context()->add_class("utilitysPanel");
    this->m_usersPanel.get_style_context()->add_class("userPanel");

    this->add(this->m_utilitysPanel);


    this->m_utilitysPanel.pack1( this->m_usersPanel , Gtk::SHRINK );
    

    this->m_pdfS = std::shared_ptr<PdfShower>( new PdfShower() );

    this->m_prompt.addPdfShower( this->m_pdfS );


    this->m_utilitysPanel.pack2( *this->m_pdfS , Gtk::FILL  );

    this->m_usersPanel.pack_start(this->m_prompt  , Gtk::PACK_SHRINK);

    if( this->atDevice()->size() > 0 )
    {
        for(auto & cont : this->atDevice()->front().get_containers() )
        {
            cont.addVersionShower( std::shared_ptr< VersionShower>( new VersionShower() ) ) ;
            cont.atVersionShower()->addMainPath( this->atMainPath());
            cont.atVersionShower()->addPdfShower( this->m_pdfS );
            this->m_nbk.append_page( *cont.atVersionShower() ,  *Gtk::manage(new Gtk::Label( cont.get_name() )) );
        }

        //propage sur tout les device les poiteur shower
        for(auto & dev : *this->atDevice() )
        {
            for(size_t i = 0 ; i < dev.get_containers().size() ; i++ )
            {
                dev.get_containers().at(i).addVersionShower( this->atDevice()->front().get_containers().at(i).atVersionShower() ) ;
                dev.get_containers().at(i).atVersionShower()->addPdfShower( this->m_pdfS );

                dev.get_containers().at(i).atVersionShower()->addCssProvider( this->getCssProvider());
            }
        }
    }

    this->m_nbk.set_scrollable();
    this->m_nbk.popup_enable();
    
    this->m_usersPanel.pack_start( this->m_nbk );
}

#endif