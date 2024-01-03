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
}

TemplateGui::~TemplateGui(void)
{
    this->m_parent = nullptr;
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
        throw std::logic_error("bad devices acces");

    return this->m_devices;
}
//acces securisé 
Glib::RefPtr<Gtk::CssProvider> const TemplateGui::atCssProvider(void)
{
    if(!this->m_cssProvider)
        throw std::logic_error("bad provider acces");

    return this->m_cssProvider;
}

//acces securisé 
Gtk::Window* const TemplateGui::atParent(void)
{
    if(this->m_parent == nullptr)
       throw std::logic_error("bad raw window acces");

    return this->m_parent;
}

void TemplateGui::addPdfShower( const std::shared_ptr< PdfShower > & _pdfS)
{
    this->m_pdfS = _pdfS;
}

const std::shared_ptr< PdfShower > &  TemplateGui::atPdfShower( void) const
{
    if(!this->m_pdfS)
        throw std::logic_error("bad PdfShower acces");

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
    Glib::RefPtr< Gtk::Adjustment > m_vadj , m_hadj;
    Gtk::VBox m_bx;

};

/// @brief initialisation de la fenetre de rendu
PdfShower::PdfShower(void ):TemplateGui(),Gtk::ScrolledWindow()
{
    this->set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    this->add(this->m_bx);
}

/// @brief lecture et affichage d'un pdf
/// @param _file chemin du ficheir pdf
void PdfShower::read_pdf( const std::string & _file )
{
    //lecture du pdf
    poppler::document *pdf_document = poppler::document::load_from_file( _file );
    
    // nettoyage de l'objet en cours
    this->clear();


    //verification de la validité du poiteur
    if( !pdf_document )
        throw std::ios_base::failure( _file + " cannot be opened");


    //lecture des différente page 
    for(auto i = 0 ; i < pdf_document->pages() ; i++)
    {
        poppler::page * pdf_page = pdf_document->create_page(i);

        if(!pdf_page)
           throw std::ios_base::failure( _file + " cannot be read page");
        

        this->m_vec_pdfImage.push_back(poppler::page_renderer().render_page(pdf_page));

        //transformation des donné brut en donné explatable gtkmm
        Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_data( (guint8*)this->m_vec_pdfImage.back().data(), Gdk::COLORSPACE_RGB, true, 8, this->m_vec_pdfImage.back().width(), this->m_vec_pdfImage.back().height(), this->m_vec_pdfImage.back().bytes_per_row() );

        //mis en memoir de la page pdf sous formt d'image
        this->m_vec_images.emplace_back(pixbuf);
        this->m_vec_images.back().show();
        
        this->m_bx.pack_start( this->m_vec_images.back() );

    }
    
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

       Gtk::VBox id  , nb_occur , m_grpInfoPath , m_grpInfoError;
       GrpVersion m_current_sel;
       Gtk::Button  m_cpyPath;
       Gtk::Label m_createDate , m_modifDate , m_autor;

       Gtk::ComboBoxText m_device_sel , m_version_sel, m_ext_sel ,m_part_sel;
       Gtk::Entry m_complet_path;

       DialogThread m_dialogInfoGrp;

       bool m_onclickLock;
};


/// @brief initilisation de la fenetre de rendu
/// @param  
VersionShower::VersionShower(void):TemplateGui(),Gtk::VBox()
{
    this->m_onclickLock = false;
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

    infoBare->attach( *Gtk::manage(new Gtk::Label("creatDate")) , 0, 1, 0, 1 );
    infoBare->attach(  *Gtk::manage(new Gtk::Label("modif date")), 1, 2, 0, 1 ); 
    infoBare->attach(  *Gtk::manage(new Gtk::Label("autor")), 2, 3, 0, 1 ); 

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
    Gtk::Table *infoGrp = Gtk::manage(new Gtk::Table( 2  , 2 ));

    infoGrp->attach( *Gtk::manage(new Gtk::Label("Path File")) , 0, 1, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);
    infoGrp->attach( *Gtk::manage(new Gtk::Label("Error")) , 1, 2, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);

    infoGrp->attach( this->m_grpInfoPath , 0, 1, 1, 2 ,Gtk::SHRINK , Gtk::SHRINK);
    infoGrp->attach( this->m_grpInfoError ,1, 2, 1, 2 ,Gtk::SHRINK , Gtk::SHRINK);

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


    this->show_all();
}


/// @brief met a jour la selection suite a un clique sur un des bouton de selection
/// @param _cpy 
void VersionShower::on_clic(GrpVersion _cpy)
{
    //deja un signal en cour
    if(this->m_onclickLock)
        return ;

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
        if(  vers.extension.size() > 0 ) 
        {   
            bool found = false;

            for(const auto & str : tmp)
            {
                if(str == vers.extension  )
                {
                    found = true ;
                    break;
                }
            }

            if(!found && !vers.get_error())
                tmp.emplace_back( vers.extension );
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
        if(  vers.version.size() > 0 ) 
        {
            bool found = false;
            for(const auto & str : tmp)
            {
                if(str == vers.version)
                {
                    found = true ;
                    break;
                }
            }

            if(!found && !vers.get_error())
                tmp.emplace_back( vers.version );
        }
    }

     for(const auto & str : tmp)
        this->m_version_sel.append(str);
     
     
    if(tmp.size() > 0)
    {
        this->m_version_sel.set_active_text( tmp.back() );
        tmp.clear();
    }
    
    
    //ajoute les partie existante
    this->m_part_sel.remove_all();
    for( const auto & vers : _cpy)
    {
        if(  vers.part.size() > 0 ) 
        {
            bool found = false;
            for(const auto & str : tmp)
            {
                if(str == vers.part)
                {
                    found = true ;
                    break;
                }
            }

            if(!found && !vers.get_error())
                tmp.emplace_back( vers.part );
        }
    }

     for(const auto & str : tmp)
        this->m_part_sel.append(str);
     
     

    if(tmp.size() > 0)
    {
        this->m_part_sel.set_active_text( tmp.back() );
        tmp.clear();
    }
    

    //completel'infobare
    

    this->m_autor.set_label( _cpy.get_autor() );
    this->m_createDate.set_label( _cpy.get_createDate() );
    this->m_modifDate.set_label( _cpy.get_createDate() );


    this->update_complet_path();

    utilitys::gtkmmRemoveChilds(this->m_grpInfoPath);
    utilitys::gtkmmRemoveChilds(this->m_grpInfoError);

    for(  auto & vers : this->m_current_sel)
    {
        auto tmp1 =Gtk::manage( new Gtk::Button(*this->atMainPath()+"\\"+ vers.get_subPathFile()));

        tmp1->get_style_context()->add_class( "VersionShowerGrpInfoPath" );
        tmp1->get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);

        this->m_grpInfoPath.pack_start(*tmp1 ,Gtk::PACK_SHRINK);

        auto tmp2 =Gtk::manage( new Gtk::Button( Version::VersionError(vers) ));

        tmp2->get_style_context()->add_class( "VersionShowerGrpInfoError" );
        tmp2->get_style_context()->add_provider( this->atCssProvider() , GTK_STYLE_PROVIDER_PRIORITY_USER);

        this->m_grpInfoError.pack_start(*tmp2,Gtk::PACK_SHRINK);
    }


    this->m_onclickLock = false;

}

/// @brief met a jour la chemin complet vers le fichier selectionné , et si un pdf , l'affiche
/// @param  
void VersionShower::update_complet_path(void)
{
    for(auto & vers : this->m_current_sel)
    {
        
        if( this->m_device_sel.get_active_text() == vers.device && "pdf" == vers.extension  && this->m_part_sel.get_active_text() == vers.part && this->m_version_sel.get_active_text() == vers.version)
        {   
            try
            {
                //lit pdf et l'affiche 
                if( vers.is_lnk())
                    this->atPdfShower()->read_pdf( vers.ptr_lnk->get_subPathFile());
                else
                    this->atPdfShower()->read_pdf(*this->atMainPath()+"\\"+ vers.get_subPathFile());
                this->atPdfShower()->show_all();
            }
            catch(const std::exception& e)
            {
                vers.add_error(Version::EF_ACCES);
                std::cerr << e.what() << '\n';
            }
        }

        ///remlpis le chemin compelt
        if( this->m_device_sel.get_active_text() == vers.device && this->m_ext_sel.get_active_text() == vers.extension  && this->m_part_sel.get_active_text() == vers.part && this->m_version_sel.get_active_text() == vers.version)
        {
            if( vers.is_lnk())
                this->m_complet_path.set_text(vers.ptr_lnk->get_subPathFile());
            else
                this->m_complet_path.set_text(*this->atMainPath()+"\\"+ vers.get_subPathFile());

            return ;
        
        }
    }

    //remet a vide le champs si rien de trouver
    this->m_complet_path.set_text("");
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

        void update_th( std::vector< std::string >::iterator const & _beg , std::vector< std::string >::iterator const & _end , std::vector<std::string > const & lsAtIgnor , std::vector<std::string > & files) ; 
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
void Prompt::update_th( std::vector< std::string >::iterator const & _beg , std::vector< std::string >::iterator const & _end , std::vector<std::string > const & lsAtIgnor , std::vector<std::string > & files)
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
                for( auto it2 = it+1 ; it2 != cont->atVersionShower()->end() ; it2++ )
                {
                    if( it->get_id() == it2->get_id() )
                    {
                        //fusionne les groupes
                        it->merge(*it2);
                        it=cont->atVersionShower()->begin()-1;
                        cont->atVersionShower()->erase(it2);
                        break;
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


    std::vector<std::string> files ;
    // Utiliser la fonction récursive pour lister l'enseble des fichiers du repertoir
    utilitys::listerFichiers(*this->atMainPath(), files);


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




#endif